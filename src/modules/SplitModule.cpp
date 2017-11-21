#include "SplitModule.h"

SplitModule::SplitModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 1, 3, 0)
{
}


void SplitModule::cycle()
{
	for (int i = 0 ; i < mNumOutputs ; ++i)
		setOutput(i, getInput(0));
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
