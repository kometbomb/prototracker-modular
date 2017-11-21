#include "AudioOutModule.h"
#include "../ModularSynth.h"

AudioOutModule::AudioOutModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 3, 0, 0)
{
}


void AudioOutModule::cycle()
{
	float leftChannel = getInput(0) + getInput(1);
	float rightChannel = getInput(0) + getInput(2);
	mSynth.setMasterOutput(0, leftChannel);
	mSynth.setMasterOutput(1, rightChannel);
}


const char * AudioOutModule::getInputName(int input) const
{
	static const char *names[] = {"L+R", "Left", "Right"};
	return names[input];
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
