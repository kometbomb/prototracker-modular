#include "DelayModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include "SDL.h"

DelayModule::DelayModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 2, 0), mHead(0), mBuffer(NULL)
{
}


DelayModule::~DelayModule()
{
	if (mBuffer != NULL)
		delete[] mBuffer;
}


void DelayModule::cycle()
{
	if (mBuffer == NULL)
		return;

	int currentReadHead = static_cast<int>(getInput(1) * mSampleRate);
	int a = std::min(currentReadHead, mPrevReadHead);
	int b = std::max(currentReadHead, mPrevReadHead);
	float avg = 0;

	mBuffer[mHead] = getInput(0) + getInput(2) * mBuffer[(mHead - currentReadHead + mMaxBufferSize) % mMaxBufferSize];

	// Dry out
	setOutput(1, getInput(0));

	for (int i = a ; i <= b ; ++i)
	{
		int readHead = (mHead - i + mMaxBufferSize) % mMaxBufferSize;
		avg += mBuffer[readHead];
	}

	setOutput(0, avg / (b - a + 1));

	++mHead;

	if (mHead >= mMaxBufferSize)
		mHead = 0;

	mPrevReadHead = currentReadHead;
}



const char * DelayModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Delay", "Feedback"};
	return names[input];
}


const char * DelayModule::getOutputName(int output) const
{
	static const char *names[] = {"Delay out", "Dry out"};
	return names[output];
}


const char * DelayModule::getName() const
{
	return "Delay";
}

SynthModule * DelayModule::createModule(ModularSynth& synth)
{
	return new DelayModule(synth);
}


void DelayModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);

	if (mBuffer != NULL)
		delete[] mBuffer;

	mMaxBufferSize = std::max(1, maxBufferSizeMs * newRate / 1000);

	mBuffer = new float[mMaxBufferSize];
	SDL_memset(mBuffer, 0, mMaxBufferSize * sizeof(mBuffer[0]));

	mPrevReadHead = static_cast<int>(getInput(1) * mSampleRate);

	mHead = 0;
}
