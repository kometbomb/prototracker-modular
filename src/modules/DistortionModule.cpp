#include "DistortionModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include "SDL.h"

DistortionModule::DistortionModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 1, 0)
{
}


DistortionModule::~DistortionModule()
{
}


void DistortionModule::cycle()
{
	float in = std::max(-1.0f, std::min(1.0f, getInput(0)));
	setOutput(0, getInput(0) - in * in * in * getInput(1));
}


const char * DistortionModule::getInputName(int input) const 
{
	static const char *names[] = {"Input", "Distortion"};
	return names[input];
}


const char * DistortionModule::getOutputName(int output) const 
{
	return "Output";
}


const char * DistortionModule::getName() const
{
	return "Distortion";
}

SynthModule * DistortionModule::createModule(ModularSynth& synth)
{
	return new DistortionModule(synth);
}
