#include "ConstModule.h"
#include <cstdio>

ConstModule::ConstModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 1, 1)
{
}


void ConstModule::cycle()
{
	setOutput(0, getParam(0));
}



const char * ConstModule::getInputName(int input) const 
{
	return "";
}


const char * ConstModule::getOutputName(int output) const 
{
	return "Output";
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
	setParam(0, static_cast<float>(delta) * 0.05 + mParams[0]);
	sprintf(mLabel, "%.2f", mParams[0]);
}


void ConstModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}
