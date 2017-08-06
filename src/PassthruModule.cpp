#include "PassthruModule.h"

PassthruModule::PassthruModule()
	:SynthModule(1, 1, 1)
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
