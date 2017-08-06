#include "OscillatorModule.h"
#include <cmath>

OscillatorModule::OscillatorModule()
	:SynthModule(1, 1, 1), mAccumulator(0.0f)
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
	return "Oscillator";
}
