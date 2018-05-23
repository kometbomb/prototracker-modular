#include "DisplayModule.h"
#include <cstdio>

DisplayModule::DisplayModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0)
{
}


void DisplayModule::cycle()
{
	sprintf(mInputValue, "%.2f", getInput(0));

	// Passthru
	setOutput(0, getInput(0));
}



const char * DisplayModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * DisplayModule::getOutputName(int output) const
{
	static const char *names[] = {"Passthru"};
	return names[output];
}


const char * DisplayModule::getName() const
{
	return mInputValue;
}

void DisplayModule::onLoaded()
{
	setInput(0, 0.0f);
	sprintf(mInputValue, "%.2f", getInput(0));
}


SynthModule * DisplayModule::createModule(ModularSynth& synth)
{
	return new DisplayModule(synth);
}
