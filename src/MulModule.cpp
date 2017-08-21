#include "MulModule.h"

MulModule::MulModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0)
{
}


void MulModule::cycle()
{
	setOutput(0, getInput(0) * getInput(1));
}



const char * MulModule::getInputName(int input) const 
{
	static const char *names[] = {"A", "B", "C"};
	return names[input];
}


const char * MulModule::getOutputName(int output) const 
{
	return "Output";
}


const char * MulModule::getName() const
{
	return "Mul";
}

SynthModule * MulModule::createModule(ModularSynth& synth)
{
	return new MulModule(synth);
}
