#include "PassthruModule.h"

PassthruModule::PassthruModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 1, 1, 0)
{
}


void PassthruModule::cycle()
{
	setOutput(0, getInput(0));
}



const char * PassthruModule::getInputName(int input) const
{
	return "Input";
}


const char * PassthruModule::getOutputName(int output) const
{
	return "Output";
}


const char * PassthruModule::getName() const
{
	return "Passthru";
}

SynthModule * PassthruModule::createModule(ModularSynth& synth)
{
	return new PassthruModule(synth);
}
