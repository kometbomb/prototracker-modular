#include "BitsModule.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "SDL.h"

BitsModule::BitsModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 2, 1, 0)
{
}


BitsModule::~BitsModule()
{
}


void BitsModule::cycle()
{
	int depth = std::max(1, 1 << static_cast<int>(getInput(1)));
	setOutput(0, trunc((getInput(0) + 1) * depth) / depth - 1);
}



const char * BitsModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Bits"};
	return names[input];
}


const char * BitsModule::getOutputName(int output) const
{
	return "Output";
}


const char * BitsModule::getName() const
{
	return "Bits";
}

SynthModule * BitsModule::createModule(ModularSynth& synth)
{
	return new BitsModule(synth);
}
