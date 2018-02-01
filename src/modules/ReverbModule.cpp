#include "ReverbModule.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include "SDL.h"

ReverbModule::ReverbModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 2, 0)
{
	for (int i = 0 ; i < numCombFilters ; ++i)
		mComb[i ]= NULL;

	for (int i = 0 ; i < numAllpassFilters ; ++i)
		mAllpass[i ]= NULL;
}


ReverbModule::~ReverbModule()
{
	for (int i = 0 ; i < numCombFilters ; ++i)
		if (mComb[i] != NULL) delete mComb[i];

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


ReverbModule::CombFilter::CombFilter(int length)
	: Filter(length)
{
}


ReverbModule::AllpassFilter::AllpassFilter(int length)
	: Filter(length)
{
}


float ReverbModule::CombFilter::cycle(float input, float gain)
{
	float output = mBuffer[mHead];
	mBuffer[mHead] = output * gain + input;

	if(++mHead >= mLength)
		mHead = 0;

	return output;
}


float ReverbModule::AllpassFilter::cycle(float input, float gain)
{
	float buffer = mBuffer[mHead];

	mBuffer[mHead] = input + buffer * gain;
	float output = -mBuffer[mHead] * gain + buffer;

	if(++mHead >= mLength)
		mHead = 0;

	return output;
}


void ReverbModule::cycle()
{
	float input = getInput(0);
	float combOut = 0;
	float gain = getInput(1);

	for (int i = 0 ; i < numCombFilters ; ++i)
	{
		combOut += mComb[i]->cycle(input, gain);
	}

	float allpassOut = combOut;

	for (int i = 0 ; i < numAllpassFilters ; ++i)
	{
		allpassOut = mAllpass[i]->cycle(allpassOut, gain);
	}

	setOutput(0, allpassOut);
	setOutput(1, input);
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

	static const float combDelayLength[numCombFilters] = { 941, 911, 887, 797, 773, 719, 701 };
	static const float allPassDelayLength[numAllpassFilters] = { 1051, 1033, 1021, 1019, 1013, 1009, 997 };

	for (int i = 0 ; i < numCombFilters ; ++i)
	{
		if (mComb[i] != NULL)
			delete mComb[i];

		mComb[i] = new CombFilter(combDelayLength[i] * newRate / 44100);
	}

	for (int i = 0 ; i < numAllpassFilters ; ++i)
	{
		if (mAllpass[i] != NULL)
			delete mAllpass[i];

		mAllpass[i] = new AllpassFilter(allPassDelayLength[i] * newRate / 44100);
	}
}
