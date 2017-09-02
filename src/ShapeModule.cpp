#include "ShapeModule.h"
#include <cmath>

ShapeModule::ShapeModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0)
{
}


void ShapeModule::cycle()
{
	float sign = getInput(0) < 0 ? -1.0f : 1.0f;
	setOutput(0, pow(fabs(getInput(0)), getInput(1)) * sign);
}



const char * ShapeModule::getInputName(int input) const 
{
	static const char *names[] = {"Input", "Shape"};
	return names[input];
}


const char * ShapeModule::getOutputName(int output) const 
{
	return "Output";
}


const char * ShapeModule::getName() const
{
	return "Shape";
}

SynthModule * ShapeModule::createModule(ModularSynth& synth)
{
	return new ShapeModule(synth);
}
