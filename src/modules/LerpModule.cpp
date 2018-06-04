#include "LerpModule.h"
#include <cstdio>
#include <algorithm>

LerpModule::LerpModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0)
{
}


void LerpModule::cycle()
{
	float a = getInput(0);
	float b = getInput(2);
	float mix = std::min(1.0f, std::max(0.0f, getInput(1)));
	setOutput(0, (b - a) * mix + a);
}



const char * LerpModule::getInputName(int input) const
{
	static const char *names[] = {"A", "Mix", "B"};
	return names[input];
}


const char * LerpModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * LerpModule::getName() const
{
	return "Lerp";
}


SynthModule * LerpModule::createModule(ModularSynth& synth)
{
	return new LerpModule(synth);
}
