#include "DelayModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include "SDL.h"

DelayModule::DelayModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 4, 2, 0), mHead(0), mBuffer(NULL)
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

	int currentReadHead = std::max(0, std::min(mMaxBufferSize - 1, static_cast<int>(getInput(1) * mSampleRate)));
	int a = std::min(currentReadHead, mPrevReadHead);
	int b = std::max(currentReadHead, mPrevReadHead);
	float avg = 0;

	// Feed input #3 and
	mBuffer[mHead] = getInput(2) * (getInput(0) + getInput(3));

	int readHead = (mHead - a + mMaxBufferSize) % mMaxBufferSize;

	for (int i = a ; i <= b ; ++i)
	{
		avg += mBuffer[readHead];

		if (++readHead >= mMaxBufferSize)
		{
			readHead = 0;
		}
	}

	// Wet out
	float wetOut = avg / (b - a + 1);
	setOutput(1, wetOut);

	// Combined out
	setOutput(0, wetOut + getInput(0));

	++mHead;

	if (mHead >= mMaxBufferSize)
		mHead = 0;

	mPrevReadHead = currentReadHead;
}



const char * DelayModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Delay", "Feedback ratio", "Feedback in"};
	return names[input];
}


const char * DelayModule::getOutputName(int output) const
{
	static const char *names[] = {"Combined out", "Wet out"};
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
