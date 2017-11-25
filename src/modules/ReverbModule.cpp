#include "ReverbModule.h"
#include "../Random.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "SDL.h"

ReverbModule::ReverbModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 2, 0), mHead(0), mBuffer(NULL)
{
	Random rnd;
	rnd.seed(rand());

	for (int i = 0 ; i < numTaps ; ++i)
	{
		mTap[i].gain = 1.0f / (1 + i);
		mTap[i].delay = static_cast<float>(i) / numTaps + rnd.rndf() * 0.9f / numTaps + 0.01f;
	}
}


ReverbModule::~ReverbModule()
{
	if (mBuffer != NULL)
	{
		delete[] mBuffer;
	}
}


void ReverbModule::cycle()
{
	mBuffer[mHead] = getInput(0);

	float delay = std::min(static_cast<float>(maxBufferSizeMs) / 1000, std::max(0.0f, getInput(1)));
	float sum = 0;

	for (int i = 0 ; i < numTaps ; ++i)
	{
		sum += mBuffer[static_cast<int>(mHead - delay * mTap[i].delay * mSampleRate + mMaxBufferSize) % mMaxBufferSize] * mTap[i].gain;
	}

	setOutput(0, sum);
	setOutput(1, getInput(0));

	++mHead;

	if (mHead >= mMaxBufferSize)
		mHead = 0;
}



const char * ReverbModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Length"};
	return names[input];
}


const char * ReverbModule::getOutputName(int output) const
{
	static const char *names[] = {"Reverb out", "Dry out"};
	return names[output];
}


const char * ReverbModule::getName() const
{
	return "Reverb";
}


SynthModule * ReverbModule::createModule(ModularSynth& synth)
{
	return new ReverbModule(synth);
}


void ReverbModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);

	if (mBuffer != NULL)
		delete[] mBuffer;

	mMaxBufferSize = std::max(1, maxBufferSizeMs * newRate / 1000);

	mBuffer = new float[mMaxBufferSize];
	SDL_memset(mBuffer, 0, mMaxBufferSize * sizeof(mBuffer[0]));

	mHead = 0;
}
