#include "SplitModule.h"

SplitModule::SplitModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 3, 0)
{
}


void SplitModule::cycle()
{
	setOutput(0, getInput(0));
	setOutput(1, getInput(0));
}



const char * SplitModule::getInputName(int input) const 
{
	return "Input";
}


const char * SplitModule::getOutputName(int output) const 
{
	static const char *names[] = {"A", "B", "C"};
	return names[output];
}


const char * SplitModule::getName() const
{
	return "Split";
}

SynthModule * SplitModule::createModule(ModularSynth& synth)
{
	return new SplitModule(synth);
}
