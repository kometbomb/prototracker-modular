#include "TickModule.h"
#include "../ModularSynth.h"
#include <cstdio>

TickModule::TickModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0)
{
}


void TickModule::cycle()
{
	// Passthru
	setOutput(0, getInput(0) / mSynth.getSongRate());
}



const char * TickModule::getInputName(int input) const
{
	static const char *names[] = {"Ticks"};
	return names[input];
}


const char * TickModule::getOutputName(int output) const
{
	static const char *names[] = {"Milliseconds"};
	return names[output];
}


const char * TickModule::getName() const
{
	return "Tick";
}


SynthModule * TickModule::createModule(ModularSynth& synth)
{
	return new TickModule(synth);
}
