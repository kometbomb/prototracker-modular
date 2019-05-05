#pragma once

#include "../SynthModule.h"
#include <cstdio>

class SampleModule: public SynthModule
{
	SampleModule(ModularSynth& synth);

  typedef struct {
    float left, right;
  } Sample;

  void reserveSampleData(int length);
  void freeSampleData();
  bool readData(const FileSection& section);
  bool writeData(FileSection& section) const;
  bool readRIFF(FILE *file);

  typedef struct {
    struct Chunk {
      char id[4];
      Uint32 size;
      void *data;

      bool readHeader(FILE *f);
      bool readData(FILE *f);
      void skip(FILE *f);

      Chunk();
      ~Chunk();
    };

    static Chunk* readRIFFChunk(FILE *file, const char *chunkName);
  } RIFF;

protected:
  Sample* mSampleData;
  int mSampleLength, mSampleRate;

public:
  ~SampleModule();

	static const int moduleId = 42;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Sample";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

  virtual bool onDataLoad(const FileSection& section);
	virtual void onDataSave(FileSection& section);
	virtual void onAction(SynthGrid& synthGrid);
 	virtual void copy(const SynthModule& source);
};
