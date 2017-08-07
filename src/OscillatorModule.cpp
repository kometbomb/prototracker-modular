#include "OscillatorModule.h"
#include <cmath>

OscillatorModule::OscillatorModule()
	:SynthModule(moduleId, 1, 1, 0), mAccumulator(0.0f)
{
}

void OscillatorModule::cycle()
{
	mAccumulator = fmod(mAccumulator + getInput(0), 1.0f);
	setOutput(0, sin(mAccumulator * (3.14157 * 2)));
}



const char * OscillatorModule::getInputName(int input) const 
{
	return "Frequency";
}


const char * OscillatorModule::getOutputName(int output) const 
{
	return "Output";
}


const char * OscillatorModule::getName() const
{
	return moduleName;
}


SynthModule * OscillatorModule::createModule()
{
	return new OscillatorModule();
}
