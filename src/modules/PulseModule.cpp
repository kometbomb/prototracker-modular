#include "PulseModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include "SDL.h"

#ifndef TUNING
#define TUNING 440.0
#endif

PulseModule::PulseModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0), mAccumulator(0), mPreviousSync(0)
{
}


PulseModule::~PulseModule()
{
}


void PulseModule::cycle()
{
	if (getInput(1) > 0.5f && mPreviousSync <= 0.5f)
		mAccumulator = 0;

	mPreviousSync = getInput(1);

	mAccumulator = fmod(mAccumulator + getInput(0) / mSampleRate * 1000, 1.0f);
	setOutput(0, mAccumulator < (getInput(2) * 0.5f + 0.5f) ? -1 : 1);
}


const char * PulseModule::getInputName(int input) const
{
	static const char *names[] = {"Frequency", "Sync", "PW"};
	return names[input];
}


const char * PulseModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * PulseModule::getName() const
{
	return "Pulse";
}

SynthModule * PulseModule::createModule(ModularSynth& synth)
{
	return new PulseModule(synth);
}
