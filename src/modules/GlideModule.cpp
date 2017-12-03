#include "GlideModule.h"
#include <algorithm>
#include <cstdio>

GlideModule::GlideModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0), mValue(0.0f)
{
}


void GlideModule::cycle()
{
	float speed = getInput(1) / mSampleRate * 1000;
	float diff = getInput(0) - mValue;
	mValue += std::max(-speed, std::min(speed, diff));

	// Passthru
	setOutput(0, mValue);
}



const char * GlideModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Speed"};
	return names[input];
}


const char * GlideModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * GlideModule::getName() const
{
	return "Glide";
}


SynthModule * GlideModule::createModule(ModularSynth& synth)
{
	return new GlideModule(synth);
}
