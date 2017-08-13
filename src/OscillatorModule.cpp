#include "OscillatorModule.h"
#include <cmath>

OscillatorModule::OscillatorModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0), mAccumulator(0.0f), mPreviousSync(0.0f)
{
}

void OscillatorModule::cycle()
{
	if (getInput(1) > 0.0f && mPreviousSync <= 0.0f)
		mAccumulator = 0;
	
	mPreviousSync = getInput(1);
	
	mAccumulator = fmod(mAccumulator + getInput(0), 1.0f);
	setOutput(0, sin((mAccumulator + getInput(2)) * (3.14157 * 2)));
}



const char * OscillatorModule::getInputName(int input) const 
{
	static const char *names[] = {"Frequency", "Sync", "Phase"};
	return names[input];
}


const char * OscillatorModule::getOutputName(int output) const 
{
	return "Output";
}


const char * OscillatorModule::getName() const
{
	return moduleName;
}


SynthModule * OscillatorModule::createModule(ModularSynth& synth)
{
	return new OscillatorModule(synth);
}
