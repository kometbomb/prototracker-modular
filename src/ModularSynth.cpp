#include "ModularSynth.h"
#include "PassthruModule.h"
#include "OscillatorModule.h"
#include "Sample.h"
#include <cstdlib>
#include <cstring>
#include <cmath>

ModularSynth::ModularSynth()
	:mNumConnections(0)
{
	for (int i = 0 ; i < maxModules ; ++i)
		mModules[i] = NULL;
	
	mInputModule = new PassthruModule();
	mOutputModule = new PassthruModule();
	mOscillatorModule = new OscillatorModule();
	mModules[0] = mInputModule;
	mModules[5] = mOscillatorModule;
	mModules[11] = mOutputModule;
	
	connectModules(0, 5, 0, 0);
	connectModules(5, 11, 0, 0);
}


ModularSynth::~ModularSynth()
{
	for (int i = 0 ; i < maxModules ; ++i)
		if (mModules[i] != NULL)
			delete mModules[i];
}

	
bool ModularSynth::connectModules(int fromModule, int toModule, int fromOutput, int toInput)
{
	if (mNumConnections >= maxConnections)
		return false;
	
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
		mNumConnections--;
		memmove(&mConnections[i], &mConnections[i + 1], sizeof(mConnections[i]) * (mNumConnections - i));
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
}


void ModularSynth::triggerNote()
{
}


void ModularSynth::setFrequency(float frequency)
{
	mInputModule->setInput(0, frequency);
}


void ModularSynth::setVolume(int volume)
{
}


void ModularSynth::update(int numSamples)
{
}
	

void ModularSynth::render(Sample16 *buffer, int numSamples, int offset)
{
	for (int i = offset ; i < numSamples ; ++i)
	{
		cycle();
		
		buffer[i].left += mOutputModule->getOutput(0) * 1000;
		buffer[i].right += mOutputModule->getOutput(0) * 1000;
	}
}


const SynthModule* ModularSynth::getModule(int index) const
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
