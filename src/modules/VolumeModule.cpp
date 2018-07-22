#include "VolumeModule.h"
#include "../ModularSynth.h"

VolumeModule::VolumeModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 1, 0)
{
}


void VolumeModule::cycle()
{
	setOutput(0, mSynth.getVolume());
}



const char * VolumeModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * VolumeModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * VolumeModule::getName() const
{
	return "Volume";
}


SynthModule * VolumeModule::createModule(ModularSynth& synth)
{
	return new VolumeModule(synth);
}
