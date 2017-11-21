#include "DelayModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include "SDL.h"

DelayModule::DelayModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 2, 1, 0), mHead(0), mBuffer(NULL)
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

	mBuffer[mHead] = getInput(0);

	++mHead;

	if (mHead >= std::max(0, std::min(mMaxBufferSize, static_cast<int>(getInput(1) * mSampleRate))))
		mHead = 0;

	setOutput(0, mBuffer[mHead]);
}



const char * DelayModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Delay"};
	return names[input];
}


const char * DelayModule::getOutputName(int output) const
{
	return "Output";
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

	mMaxBufferSize = maxBufferSizeMs * newRate / 1000;

	mBuffer = new float[mMaxBufferSize];
	SDL_memset(mBuffer, 0, mMaxBufferSize * sizeof(mBuffer[0]));

	mHead = 0;
}
