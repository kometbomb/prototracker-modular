#include "IntModule.h"
#include <cstdio>

IntModule::IntModule(ModularSynth& synth)
:SynthModule(synth, moduleId, 0, 1, 1)
{
}


void IntModule::cycle()
{
	setOutput(0, getParam(0));
}



const char * IntModule::getInputName(int input) const
{
	return "";
}


const char * IntModule::getOutputName(int output) const
{
	return "Output";
}


const char * IntModule::getName() const
{
	return mLabel;
}


SynthModule * IntModule::createModule(ModularSynth& synth)
{
	return new IntModule(synth);
}


void IntModule::onDial(int delta)
{
	setParam(0, static_cast<float>(delta) + mParams[0]);
	snprintf(mLabel, sizeof(mLabel), "%.2f", mParams[0]);
}


void IntModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}

void IntModule::onShow()
{
	// Trigger mLabel update
	onDial(0);
}
