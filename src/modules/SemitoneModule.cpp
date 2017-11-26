#include "SemitoneModule.h"
#include <cstdio>
#include <cmath>

SemitoneModule::SemitoneModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0)
{
}


void SemitoneModule::cycle()
{
	static const float ratio = powf(2.0f, 1.0f/12.0f);
	float frequency = powf(ratio, getInput(0));

	// Set relative frequency (input 0 = 1.0, 1 = +1 semitone)
	setOutput(0, frequency);
}



const char * SemitoneModule::getInputName(int input) const
{
	static const char *names[] = {"Rel. note"};
	return names[input];
}


const char * SemitoneModule::getOutputName(int output) const
{
	static const char *names[] = {"Ratio"};
	return names[output];
}


const char * SemitoneModule::getName() const
{
	return "Semitone";
}


SynthModule * SemitoneModule::createModule(ModularSynth& synth)
{
	return new SemitoneModule(synth);
}
