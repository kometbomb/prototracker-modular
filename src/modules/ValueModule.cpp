#include "ValueModule.h"
#include <cstdio>

ValueModule::ValueModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0)
{
}


void ValueModule::cycle()
{
//	mInputValue = getInput(0); <- this didn't work. i am stupid
	sprintf(mInputValue, "%.2f", getInput(0));

	// Passthru
	setOutput(0, getInput(0));
}



const char * ValueModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * ValueModule::getOutputName(int output) const
{
	static const char *names[] = {"Passthru"};
	return names[output];
}


const char * ValueModule::getName() const
{
	return mInputValue;
}

void ValueModule::onLoaded()
{
	setInput(0, 0.0f);
	sprintf(mInputValue, "%.2f", getInput(0));
}


SynthModule * ValueModule::createModule(ModularSynth& synth)
{
	return new ValueModule(synth);
}
