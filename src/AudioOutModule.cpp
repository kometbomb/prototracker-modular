#include "AudioOutModule.h"
#include "ModularSynth.h"

AudioOutModule::AudioOutModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 0, 0)
{
}


void AudioOutModule::cycle()
{
	// Only left channel! (mono)
	mSynth.setMasterOutput(0, getInput(0));
}


const char * AudioOutModule::getInputName(int input) const 
{
	return "AudioOut";
}


const char * AudioOutModule::getOutputName(int output) const 
{
	return "Output";
}


const char * AudioOutModule::getName() const
{
	return "AudioOut";
}

SynthModule * AudioOutModule::createModule(ModularSynth& synth)
{
	return new AudioOutModule(synth);
}
