#include "SampleModule.h"
#include "../FileSection.h"
#include "../SynthGrid.h"
#include "../Debug.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstring>

SampleModule::SampleModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 4, 0, true),
  mSampleData(NULL), mSampleLength(0), mSampleRate(0)
{
}


SampleModule::~SampleModule()
{
  freeSampleData();
}


void SampleModule::reserveSampleData(int length)
{
  freeSampleData();

  mSampleData = new Sample[mSampleLength];
  mSampleLength = length;
}


void SampleModule::freeSampleData()
{
  if (mSampleData != NULL)
  {
    delete[] mSampleData;
    mSampleData = NULL;
  }
}


void SampleModule::cycle()
{
  if (mSampleLength <= 0)
  {
    return;
  }

  const float index = fabs(getInput(0));
  const float sampleIndex = floor(mSampleLength * index);
  const Sample& currentSample = mSampleData[std::min(static_cast<int>(sampleIndex), mSampleLength - 1)];
  const float left = currentSample.left;
  const float right = currentSample.right;

  setOutput(0, left + right);
  setOutput(1, left);
  setOutput(2, right);
  setOutput(3, static_cast<float>(mSampleLength) / std::max(1, mSampleRate));
}


const char * SampleModule::getInputName(int input) const
{
	static const char *names[] = {"Position"};
	return names[input];
}


const char * SampleModule::getOutputName(int output) const
{
	static const char *names[] = {"L+R", "Left", "Right", "Length"};
	return names[output];
}


const char * SampleModule::getName() const
{
	return "Sample";
}


SynthModule * SampleModule::createModule(ModularSynth& synth)
{
	return new SampleModule(synth);
}


bool SampleModule::readData(const FileSection& section)
{
  int offset = 0;
  int length = section.readDword(offset);
  int sampleRate;

  if (length == FileSection::invalidRead)
  {
    return false;
  }

  mSampleLength = length;

  sampleRate = section.readDword(offset);

  if (sampleRate == FileSection::invalidRead)
  {
    return false;
  }

  mSampleRate = sampleRate;

  reserveSampleData(length);

  for (int channel = 0 ; channel < 2 ; ++channel)
  {
    float accumulator = 0;

    for (int i = 0 ; i < mSampleLength ; ++i)
    {
      float amp = section.readFloat(offset);

      if (amp == NAN)
      {
        return false;
      }

      accumulator += amp;

      if (channel == 0)
      {
        mSampleData[i].left = accumulator;
      }
      else
      {
        // Delta against left channel
        mSampleData[i].right = accumulator + mSampleData[i].left;
      }
    }
  }

  return true;
}


bool SampleModule::readRIFF(FILE *file)
{
  typedef struct
	{
		Uint16 wFormatTag;
		Uint16 nChannels;
		Uint32 nSamplesPerSec;
		Uint32 nAvgBytesPerSec;
		Uint16 nBlockAlign;
		Uint16 wBitsPerSample;
		/*----*/
		Uint16 cbSize;
		/*----*/
		Uint16 wValidBitsPerSample;
		Uint32 dwChannelMask;
		char SubFormat[16];
	} __attribute__((__packed__)) fmtData;

  bool success = false;
  const int WAVE_FORMAT_PCM = 1;
  const int WAVE_FORMAT_IEEE_FLOAT = 3;

  RIFF::Chunk *main = new RIFF::Chunk();

  if (main->readHeader(file))
  {
    if (strncmp("RIFF", main->id, 4) == 0)
    {
      // Skip main RIFF header, assume it's a wave
      fseek(file, 12, SEEK_CUR);

      size_t originalPosition = ftell(file);
      RIFF::Chunk *fmtChunk = RIFF::readRIFFChunk(file, "fmt ");
      fseek(file, originalPosition, SEEK_SET);

      if (fmtChunk != NULL)
      {
        fmtData* fmt = reinterpret_cast<fmtData*>(fmtChunk->data);

        // PCM integer or PCM float

        if (fmt->wFormatTag == WAVE_FORMAT_PCM || fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        {
          fmtData* fmt = reinterpret_cast<fmtData*>(fmtChunk->data);
          RIFF::Chunk *dataChunk = RIFF::readRIFFChunk(file, "data");

          if (dataChunk != NULL)
          {
            size_t stride = (fmt->wBitsPerSample / 8) / fmt->nChannels;
            mSampleRate = fmt->nSamplesPerSec;
            mSampleLength = dataChunk->size / stride;

            reserveSampleData(mSampleLength);

            for (int i = 0 ; i < mSampleLength ; ++i)
            {
              for (int channel = 0 ; channel < fmt->nChannels ; ++channel)
              {
                float sample;
                if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
                {
                  if (fmt->wBitsPerSample == 32)
                  {
                    sample = std::max(-1.0f, std::min(1.0f, reinterpret_cast<float*>(dataChunk->data)[i * fmt->nChannels + channel]));
                  }
                  else
                  {
                    sample = std::max(-1.0, std::min(1.0, reinterpret_cast<double*>(dataChunk->data)[i * fmt->nChannels + channel]));
                  }
                }
                else
                {
                  if (fmt->wBitsPerSample == 16)
                  {
                    sample = static_cast<float>(reinterpret_cast<Sint16*>(dataChunk->data)[i * fmt->nChannels + channel]) / 32768;
                  }
                  else if (fmt->wBitsPerSample == 32)
                  {
                    sample = std::max(-1.0f, std::min(1.0f, static_cast<float>(reinterpret_cast<Sint32*>(dataChunk->data)[i * fmt->nChannels + channel]) / 32768));
                  }
                  else
                  {
                    sample = static_cast<float>(reinterpret_cast<Uint8*>(dataChunk->data)[i * fmt->nChannels + channel]) / 128 - 1.0f;
                  }
                }

                if (fmt->nChannels == 1)
                {
                  mSampleData[i].left = sample;
                  mSampleData[i].right = sample;
                }
                else if (channel == 0)
                {
                  mSampleData[i].left = sample;
                }
                else if (channel == 1)
                {
                  mSampleData[i].right = sample;
                }
              }
            }

            success = true;
            delete dataChunk;
          }
        }
      }

      delete fmtChunk;
    }

    delete main;
  }

  return success;
}


bool SampleModule::writeData(FileSection& section) const
{
  section.writeDword(mSampleLength);
  section.writeDword(mSampleRate);

  float amp = 0, prevAmp = 0;

  for (int channel = 0 ; channel < 2 ; ++channel)
  {
    for (int i = 0 ; i < mSampleLength ; ++i)
    {
      prevAmp = amp;

      if (channel == 0)
      {
        amp = mSampleData[i].left;
      }
      else
      {
        // Delta against left channel
        amp = mSampleData[i].right - mSampleData[i].left;
      }

      section.writeFloat(amp - prevAmp);
    }
  }

  return true;
}


SampleModule::RIFF::Chunk::Chunk()
  : size(0), data(NULL)
{

}


SampleModule::RIFF::Chunk::~Chunk()
{
  if (data != NULL)
  {
    free(data);
  }
}


bool SampleModule::RIFF::Chunk::readHeader(FILE *file)
{
  size_t originalPosition = ftell(file);

  if (fread(id, sizeof(id), 1, file) != 1)
  {
    return false;
  }

  if (fread(&size, sizeof(size), 1, file) != 1)
  {
    return false;
  }

  fseek(file, originalPosition, SEEK_SET);

  return true;
}


bool SampleModule::RIFF::Chunk::readData(FILE *file)
{
  // Skip header
  fseek(file, 8, SEEK_CUR);

  data = malloc(size);

  if (fread(data, size, 1, file) != 1)
  {
    free(data);
    data = NULL;
    return false;
  }

  return true;
}


void SampleModule::RIFF::Chunk::skip(FILE *file)
{
  fseek(file, 8 + size, SEEK_CUR);
}


SampleModule::RIFF::Chunk* SampleModule::RIFF::readRIFFChunk(FILE *file, const char *chunkName)
{
  Chunk* chunk = new Chunk();

  while (chunk->readHeader(file))
  {
    if (strncmp(chunk->id, chunkName, 4) == 0)
    {
      if (chunk->readData(file))
      {
        return chunk;
      }
      else
      {
        break;
      }
    }

    chunk->skip(file);
  }

  delete chunk;

  return NULL;
}


bool SampleModule::onDataLoad(const FileSection& section)
{
  return readData(section);
}


void SampleModule::onDataSave(FileSection& section)
{
  writeData(section);
}


void SampleModule::onAction(SynthGrid& synthGrid)
{
  synthGrid.displayFileSelectionDialog("Load sample", "wav", [this, &synthGrid](FILE *f) {
    if (!readRIFF(f))
    {
      // message
    }
  });
}


void SampleModule::copy(const SynthModule& source)
{
  reserveSampleData(static_cast<const SampleModule&>(source).mSampleLength);
  memcpy(mSampleData, static_cast<const SampleModule&>(source).mSampleData, mSampleLength * sizeof(mSampleData[0]));
}
