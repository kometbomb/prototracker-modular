#include "RMSModule.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "SDL.h"

RMSModule::RMSModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0), mBuffer(NULL), mSum(0), mHead(0)
{
	initBuffer();
}


RMSModule::~RMSModule()
{
	delete[] mBuffer;
}


void RMSModule::initBuffer()
{
	if (mBuffer != NULL)
	{
		delete[] mBuffer;
		mBuffer = NULL;
	}

	if (mSampleRate > 0)
	{
		mBufferLength = mSampleRate * bufferLengthMs / 1000;
		mBuffer = new float[mBufferLength];
		SDL_memset(mBuffer, 0, sizeof(mBuffer[0]) * mBufferLength);
	}

	mHead = 0;
	mSum = 0;
}


void RMSModule::cycle()
{
	float input = getInput(0);
	input *= input;

	mSum += input;
	mSum -= mBuffer[mHead];
	mBuffer[mHead] = input;

	if (++mHead >= mBufferLength)
	{
		mHead = 0;
	}

	setOutput(0, sqrt(std::max(0.0f, mSum / mBufferLength)));
}


const char * RMSModule::getInputName(int input) const
{
	return "Input";
}


const char * RMSModule::getOutputName(int output) const
{
	return "Amplitude";
}


const char * RMSModule::getName() const
{
	return "RMS";
}

SynthModule * RMSModule::createModule(ModularSynth& synth)
{
	return new RMSModule(synth);
}


void RMSModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);
	initBuffer();
}
