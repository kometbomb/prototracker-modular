#include "AbsModule.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "SDL.h"

AbsModule::AbsModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 1, 1, 0)
{
}


AbsModule::~AbsModule()
{
}


void AbsModule::cycle()
{
	setOutput(0, fabs(getInput(0)));
}



const char * AbsModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * AbsModule::getOutputName(int output) const
{
	return "Output";
}


const char * AbsModule::getName() const
{
	return "Abs";
}

SynthModule * AbsModule::createModule(ModularSynth& synth)
{
	return new AbsModule(synth);
}
