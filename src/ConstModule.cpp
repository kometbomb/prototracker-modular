#include "ConstModule.h"

ConstModule::ConstModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 1, 0)
{
}


void ConstModule::cycle()
{
	setOutput(0, 0.25f);
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
	return "Const";
}

SynthModule * ConstModule::createModule(ModularSynth& synth)
{
	return new ConstModule(synth);
}
