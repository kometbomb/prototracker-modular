#include "AddModule.h"

AddModule::AddModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0)
{
}


void AddModule::cycle()
{
	float sum = 0.0f;
	for (int i = 0 ; i < mNumInputs	; ++i)
		sum += getInput(i);
	setOutput(0, sum);
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
