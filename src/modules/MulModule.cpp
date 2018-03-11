#include "MulModule.h"

MulModule::MulModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 2, 0)
{
}


void MulModule::cycle()
{
	setOutput(0, getInput(0) * getInput(1));
	setOutput(1, getInput(2) * getInput(1));
}



const char * MulModule::getInputName(int input) const
{
	static const char *names[] = {"A", "Multiplier", "B"};
	return names[input];
}


const char * MulModule::getOutputName(int output) const
{
	static const char *names[] = {"Mul A", "Mul B"};
	return names[output];
}


const char * MulModule::getName() const
{
	return "Mul";
}

SynthModule * MulModule::createModule(ModularSynth& synth)
{
	return new MulModule(synth);
}
