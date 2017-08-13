#include "AddModule.h"

AddModule::AddModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0)
{
}


void AddModule::cycle()
{
	setOutput(0, getInput(0) + getInput(1));
}



const char * AddModule::getInputName(int input) const 
{
	static const char *names[] = {"A", "B", "C"};
	return names[input];
}


const char * AddModule::getOutputName(int output) const 
{
	return "Output";
}


const char * AddModule::getName() const
{
	return "Add";
}

SynthModule * AddModule::createModule(ModularSynth& synth)
{
	return new AddModule(synth);
}
