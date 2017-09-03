#include "AccumulatorModule.h"
#include <algorithm>

AccumulatorModule::AccumulatorModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0), mAccumulator(0.0f), mPreviousSync(0.0f)
{
}


void AccumulatorModule::cycle()
{
	mAccumulator += getInput(0) / mSampleRate;
	
	if (getInput(1) > 0.0f && mPreviousSync <= 0.0f)
		mAccumulator = getInput(2);
	
	mPreviousSync = getInput(1);
	
	setOutput(0, mAccumulator);
}



const char * AccumulatorModule::getInputName(int input) const 
{
	static const char *names[] = {"Add", "Reset", "Value"};
	return names[input];
}


const char * AccumulatorModule::getOutputName(int output) const 
{
	return "Output";
}


const char * AccumulatorModule::getName() const
{
	return "Accumulator";
}

SynthModule * AccumulatorModule::createModule(ModularSynth& synth)
{
	return new AccumulatorModule(synth);
}
