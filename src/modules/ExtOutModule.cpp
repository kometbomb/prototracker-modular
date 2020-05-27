#include "ExtOutModule.h"
#include "../ModularSynth.h"
#include <cstdio>

ExtOutModule::ExtOutModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 0, 1)
{
	setParam(0, 0.0f);
}


void ExtOutModule::cycle()
{
	int index = static_cast<int>(mParams[0]) % ModularSynth::numExtConnections;
	mSynth.setExtOutput(index, getInput(0));
}


const char * ExtOutModule::getInputName(int input) const
{
	static const char *names[] = {"ExtOut"};
	return names[input];
}


const char * ExtOutModule::getOutputName(int output) const
{
	return NULL;
}


const char * ExtOutModule::getName() const
{
	return mLabel;
}


SynthModule * ExtOutModule::createModule(ModularSynth& synth)
{
	return new ExtOutModule(synth);
}


void ExtOutModule::onDial(int delta)
{
	int index = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + ModularSynth::numExtConnections) % ModularSynth::numExtConnections;
	setParam(0, index);
	sprintf(mLabel, "Out%d", index);
}


void ExtOutModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}
