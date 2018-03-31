#include "ReverbModule.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "SDL.h"

ReverbModule::ReverbModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 2, 0), mDelayOffset(-1)
{
	for (int i = 0 ; i < numDelays ; ++i)
		mDelay[i]= NULL;

	for (int i = 0 ; i < numAllpassFilters ; ++i)
		mAllpass[i]= NULL;
}


ReverbModule::~ReverbModule()
{
	for (int i = 0 ; i < numDelays ; ++i)
		if (mDelay[i] != NULL) delete mDelay[i];

	for (int i = 0 ; i < numAllpassFilters ; ++i)
		if (mAllpass[i] != NULL) delete mAllpass[i];
}


ReverbModule::Filter::Filter(int length)
	:mLength(length), mHead(0)
{
	mBuffer = new float[length];
	SDL_memset(mBuffer, 0, sizeof(float) * length);
}


ReverbModule::Filter::~Filter()
{
	delete[] mBuffer;
}


ReverbModule::AllpassFilter::AllpassFilter(int length)
	: Filter(length)
{
}


float ReverbModule::AllpassFilter::cycle(float input, float gain)
{
	float buffer = mBuffer[mHead];
	float output = input * gain + buffer;
	mBuffer[mHead] = input + output * -gain;

	if(++mHead >= mLength)
  		mHead = 0;

	return output;
}


ReverbModule::DelayFilter::DelayFilter(int length)
	: Filter(length), mOutput(0.0f)
{
}


float ReverbModule::DelayFilter::cycle(float input)
{
	float output = mBuffer[mHead];
	mBuffer[mHead] = input;

	if(++mHead >= mLength)
  		mHead = 0;

	mOutput = output;

	return output;
}


float ReverbModule::DelayFilter::getOutput() const
{
	return mOutput;
}


void ReverbModule::cycle()
{
	float input = getInput(0);
	float Krt = getInput(1);

	float newOffset = std::max(0.0f, std::min(1.0f, getInput(2)));

	// Reinit delays if delay offset changes

	if (mDelayOffset != newOffset)
	{
		mDelayOffset = newOffset;
		initDelays(mDelayOffset);
	}

	float output = 0.0f;
	float amp = mDelay[numDelays - 1]->getOutput() * Krt;
	int allPassIdx = 0;

	for (int row = 0 ; row < numRows ; ++row)
	{
		amp += input;

		for (int allpass = 0 ; allpass < numAllpassFiltersPerRow ; ++allpass)
		{
			amp = mAllpass[allPassIdx++]->cycle(amp, 0.5f);
		}

		amp = mDelay[row]->cycle(amp) * Krt;
		output += amp;
	}

	setOutput(0, input + output / numRows);
	setOutput(1, output / numRows);
}



const char * ReverbModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Reverb time", "Delay offset"};
	return names[input];
}


const char * ReverbModule::getOutputName(int output) const
{
	static const char *names[] = {"Combined out", "Wet out"};
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


void ReverbModule::initDelays(float offsetTime)
{
	static const float delayLength[numDelays] = { 0.00733f, 0.00683f, 0.00757f, 0.00743f };
	static const float allPassDelayLength[numAllpassFilters] = { 0.04591f, 0.06337f, 0.04597f, 0.05479f, 0.04651f, 0.03217f, 0.04643f, 0.06521f };

	for (int i = 0 ; i < numDelays ; ++i)
	{
		if (mDelay[i] != NULL)
			delete mDelay[i];

		mDelay[i] = new DelayFilter(delayLength[i] * mSampleRate);
	}

	for (int i = 0 ; i < numAllpassFilters ; ++i)
	{
		if (mAllpass[i] != NULL)
			delete mAllpass[i];

		mAllpass[i] = new AllpassFilter((allPassDelayLength[i] + offsetTime) * mSampleRate);
	}
}


void ReverbModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);

	if (mDelayOffset >= 0.0f)
		initDelays(mDelayOffset);
}
