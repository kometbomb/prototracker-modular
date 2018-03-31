#include "ModularSynth.h"
#include "ModuleFactory.h"
#include "Sample.h"
#include "FileSection.h"
#include "ModuleFactory.h"
#include "TrackState.h"
#include "IPlayer.h"
#include "SDL.h"
#include "Synth.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#ifndef TUNING
#define TUNING 440.0
#endif

ModularSynth::ModularSynth(Synth& synth, IPlayer& player)
	: mSynth(synth), mPlayer(player), mNumConnections(0), mFrequency(0), mVolume(0), mNoteTrigger(false)
{
	for (int i = 0 ; i < maxModules ; ++i)
		mModules[i] = NULL;

	SDL_memset(mOutput, 0, sizeof(mOutput));
}


ModularSynth::~ModularSynth()
{
	clear();
}


bool ModularSynth::addModule(int index, int moduleId)
{
	ModuleFactory moduleFactory;

	if (mModules[index] != NULL)
		return false;

	mModules[index] = moduleFactory.createModule(moduleId, *this);
	mModules[index]->setSampleRate(mSampleRate);
	mModules[index]->onLoaded();

	return true;
}


bool ModularSynth::connectModules(int fromModule, int toModule, int fromOutput, int toInput)
{
	if (mNumConnections >= maxConnections)
		return false;

	for (int i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];

		if (connection.toModule == toModule && connection.toInput == toInput)
			return false;

		if (connection.fromModule == fromModule && connection.fromOutput == fromOutput)
			return false;
	}

	SynthConnection& connection = mConnections[mNumConnections];
	connection.fromModule = fromModule;
	connection.toModule = toModule;
	connection.fromOutput = fromOutput;
	connection.toInput = toInput;

	mNumConnections++;

	return true;
}


void ModularSynth::detachConnection(int moduleIndex, int type, int connectionIndex)
{
	int i;

	for (i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];
		if (type == 1)
		{
			if (connection.fromModule == moduleIndex && connection.fromOutput == connectionIndex)
				break;
		}
		else
		{
			if (connection.toModule == moduleIndex && connection.toInput == connectionIndex)
				break;
		}
	}

	if (i < mNumConnections)
	{
		removeConnection(i);
	}
}


void ModularSynth::removeConnection(int index)
{
	if (index < mNumConnections)
	{
		SynthConnection& connection = mConnections[index];
		if (mModules[connection.toModule] != NULL)
			mModules[connection.toModule]->setInput(connection.toInput, 0.0f);

		mNumConnections--;
		memmove(&mConnections[index], &mConnections[index + 1], sizeof(mConnections[index]) * (mNumConnections - index));
	}
}


void ModularSynth::cycle()
{
	for (int i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];
		mModules[connection.toModule]->setInput(connection.toInput, mModules[connection.fromModule]->getOutput(connection.fromOutput));
	}

	for (int i = 0 ; i < maxModules ; ++i)
		if (mModules[i] != NULL)
			mModules[i]->cycle();

	// After all modules have had their change to see if note was triggered,
	// reset the status
	mNoteTrigger = false;
}


void ModularSynth::triggerNote()
{
	mNoteTrigger = true;
}


void ModularSynth::setFrequency(float frequency)
{
	mFrequency = frequency * mSampleRate / static_cast<float>(TUNING);
}


void ModularSynth::setVolume(int volume)
{
	mVolume = static_cast<float>(volume) / ITrackState::maxVolume;
}


void ModularSynth::update(int numSamples)
{
}


void ModularSynth::render(Sample16 *buffer, int numSamples, int offset)
{
	lock();

	// No update or output if volume is zero, meaning the channel is muted
	if (mVolume > 0.0f)
	{
		for (int i = offset ; i < numSamples ; ++i)
		{
			cycle();

			buffer[i].left += mOutput[0] * outputResolution;
			buffer[i].right += mOutput[1] * outputResolution;
		}
	}

	unlock();
}


const SynthModule* ModularSynth::getModule(int index) const
{
	return mModules[index];
}


SynthModule* ModularSynth::getModule(int index)
{
	return mModules[index];
}


const SynthConnection& ModularSynth::getConnection(int index) const
{
	return mConnections[index];
}


int ModularSynth::getNumConnections() const
{
	return mNumConnections;
}


void ModularSynth::swapModules(int fromModule, int toModule)
{
	SynthModule* temp = mModules[fromModule];
	mModules[fromModule] = mModules[toModule];
	mModules[toModule] = temp;

	for (int i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];

		if (connection.fromModule == toModule)
			connection.fromModule = fromModule;
		else if (connection.fromModule == fromModule)
			connection.fromModule = toModule;

		if (connection.toModule == toModule)
			connection.toModule = fromModule;
		else if (connection.toModule == fromModule)
			connection.toModule = toModule;
	}
}


void ModularSynth::removeModule(int index)
{
	if (mModules[index] != NULL)
	{
		delete mModules[index];
		mModules[index] = NULL;

		// Remove any connections from/to this module

		for (int i = 0 ; i < mNumConnections ; )
		{
			SynthConnection& connection = mConnections[i];

			if (connection.fromModule == index || connection.toModule == index)
				removeConnection(i);
			else
				++i;
		}
	}
}


/* SynthModule parameters are scaled so that -1.0 .. 1.0 equals 0x3fff000 0x4001000
 * which should be accurate and deep enough for any normal purposes (values mostly between
 * 0-255).
 */

static const int paramScale =  0x0001000;
static const int paramCenter = 0x4000000;

bool ModularSynth::readSynth(const FileSection& section, int& offset)
{
	int countModules = section.readByte(offset);

	if (countModules == FileSection::invalidRead || countModules > maxModules)
		return false;

	ModuleFactory moduleFactory;

	clear();

	for (int i = 0 ; i < countModules ; ++i)
	{
		int synthId = section.readByte(offset);

		if (synthId != 0)
		{
			SynthModule *module = moduleFactory.createModule(synthId, *this);

			if (!module)
				return false;

			for (int p = 0 ; p < module->getNumParams() ; ++p)
			{
				int param = section.readDword(offset);
				module->setParam(p, static_cast<float>(param - paramCenter) / paramScale);
			}

			if (module->getHasData())
			{
				FileSection *moduleData = section.readSection(offset);
				module->onDataLoad(*moduleData);
				delete moduleData;
			}

			mModules[i] = module;
			module->setSampleRate(mSampleRate);
			module->onLoaded();
		}
	}

	int countConnections = section.readByte(offset);

	if (countConnections == FileSection::invalidRead || countConnections > maxConnections)
		return false;

	mNumConnections = countConnections;

	for (int i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];

		int temp = section.readByte(offset);

		if (temp == FileSection::invalidRead || temp > maxModules)
			return false;

		connection.fromModule = temp;

		temp = section.readByte(offset);

		if (temp == FileSection::invalidRead || temp > maxModules)
			return false;
		connection.toModule = temp;

		temp = section.readByte(offset);

		if (temp == FileSection::invalidRead || temp > maxModules)
			return false;

		connection.fromOutput = temp;

		temp = section.readByte(offset);

		if (temp == FileSection::invalidRead || temp > maxModules)
			return false;

		connection.toInput = temp;
	}

	return true;
}


void ModularSynth::writeSynth(FileSection& section)
{
	// Write modules

	section.writeByte(maxModules);

	for (int i = 0 ; i < maxModules ; ++i)
	{
		if (mModules[i] != NULL)
		{
			section.writeByte(mModules[i]->getSynthId());

			for (int p = 0 ; p < mModules[i]->getNumParams() ; ++p)
			{
				section.writeDword(mModules[i]->getParam(p) * paramScale + paramCenter);
			}

			if (mModules[i]->getHasData())
			{
				FileSection *moduleData = FileSection::createSection("DATA");
				mModules[i]->onDataSave(*moduleData);
				section.writeSection(*moduleData);
				delete moduleData;
			}
		}
		else
		{
			section.writeByte(0);
		}
	}

	// Write connections

	section.writeByte(mNumConnections);

	for (int i = 0 ; i < mNumConnections ; ++i)
	{
		SynthConnection& connection = mConnections[i];
		section.writeByte(connection.fromModule);
		section.writeByte(connection.toModule);
		section.writeByte(connection.fromOutput);
		section.writeByte(connection.toInput);
	}
}


void ModularSynth::clear()
{
	for (int i = 0 ; i < maxModules ; ++i)
	{
		if (mModules[i] != NULL)
		{
			delete mModules[i];
			mModules[i] = NULL;
		}
	}

	mNumConnections = 0;
}


float ModularSynth::getFrequency() const
{
	return mFrequency;
}


void ModularSynth::setMasterOutput(int channel, float output)
{
	mOutput[channel] = output;
}


bool ModularSynth::getNoteTrigger() const
{
	if (mNoteTrigger)
		return 1.0f;
	else
		return 0.0f;
}


void ModularSynth::setSampleRate(int rate)
{
	mSampleRate = rate;
	for (int index = 0 ; index < maxModules ; ++index)
	{
		if (mModules[index] != NULL)
			mModules[index]->setSampleRate(mSampleRate);
	}
}


void ModularSynth::copy(const ModularSynth& source)
{
	for (int i = 0 ; i < maxModules ; ++i)
	{
		if (source.getModule(i) != NULL)
		{
			addModule(i, source.getModule(i)->getSynthId());
			SynthModule *newModule = getModule(i);

			for (int p = 0 ; p < newModule->getNumParams() ; ++p)
			{
				newModule->setParam(p, source.getModule(i)->getParam(p));
			}
		}
		else
		{
			if (getModule(i) != NULL)
				removeModule(i);
		}
	}

	mNumConnections = 0;

	for (int i = 0 ; i < source.getNumConnections() ; ++i)
	{
		const SynthConnection& connection = source.getConnection(i);
		connectModules(connection.fromModule, connection.toModule, connection.fromOutput, connection.toInput);
	}
}


ModularSynth* ModularSynth::clone() const
{
	ModularSynth *newSynth = new ModularSynth(mSynth, mPlayer);
	newSynth->copy(*this);

	return newSynth;
}


void ModularSynth::handleTrackState(ITrackState& _trackState)
{
	TrackState& trackState = static_cast<TrackState&>(_trackState);
	SDL_memcpy(mEffectValues, trackState.effectValues, sizeof(mEffectValues));
}


int ModularSynth::getEffectValue(int effect) const
{
	return mEffectValues[effect];
}


float ModularSynth::getAutomationValue(int track) const
{
	return mSynth.getAutomationValue(track);
}


int ModularSynth::getSongRate() const
{
	return mPlayer.getPlayerState().songRate;
}
