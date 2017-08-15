#include "ClampModule.h"
#include <algorithm>

ClampModule::ClampModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0)
{
}


void ClampModule::cycle()
{
	setOutput(0, std::min(std::max(getInput(0), getInput(1)), getInput(2)));
}



const char * ClampModule::getInputName(int input) const 
{
	static const char *names[] = {"Input", "Min", "Max"};
	return names[input];
}


const char * ClampModule::getOutputName(int output) const 
{
	return "Output";
}


const char * ClampModule::getName() const
{
	return "Clamp";
}

SynthModule * ClampModule::createModule(ModularSynth& synth)
{
	return new ClampModule(synth);
}
