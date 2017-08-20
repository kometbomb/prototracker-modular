#include "FrequencyInModule.h"
#include "ModularSynth.h"

FrequencyInModule::FrequencyInModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 2, 0)
{
}

void FrequencyInModule::cycle()
{
	setOutput(0, mSynth.getFrequency());
	setOutput(1, mSynth.getFrequency() * mSampleRate);
}



const char * FrequencyInModule::getInputName(int input) const 
{
	return "FrequencyIn";
}


const char * FrequencyInModule::getOutputName(int output) const 
{
	static const char *names[] = {"Output", "SOutput"};
	return names[output];
}


const char * FrequencyInModule::getName() const
{
	return "FrequencyIn";
}

SynthModule * FrequencyInModule::createModule(ModularSynth& synth)
{
	return new FrequencyInModule(synth);
}
