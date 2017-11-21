#include "ConstModule.h"
#include <cstdio>

ConstModule::ConstModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 0, 2, 1)
{
}


void ConstModule::cycle()
{
	// 1.0 * const
	setOutput(0, getParam(0));

	// 0.001 * const
	setOutput(1, getParam(0) / 1000.0f);
}



const char * ConstModule::getInputName(int input) const
{
	return "";
}


const char * ConstModule::getOutputName(int output) const
{
	static const char *names[] = {"Output", "Output/1000"};
	return names[output];
}


const char * ConstModule::getName() const
{
	return mLabel;
}


SynthModule * ConstModule::createModule(ModularSynth& synth)
{
	return new ConstModule(synth);
}


void ConstModule::onDial(int delta)
{
	setParam(0, static_cast<float>(delta) * 0.01 + mParams[0]);
	sprintf(mLabel, "%.2f", mParams[0]);
}


void ConstModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}
