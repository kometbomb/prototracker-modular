#include "DivModule.h"
#include <algorithm>

DivModule::DivModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0)
{
}


void DivModule::cycle()
{
	float divisor = std::max(0.0001f, getInput(1));
	setOutput(0, getInput(0) / divisor);
}


const char * DivModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Div"};
	return names[input];
}


const char * DivModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * DivModule::getName() const
{
	return "Div";
}


SynthModule * DivModule::createModule(ModularSynth& synth)
{
	return new DivModule(synth);
}
