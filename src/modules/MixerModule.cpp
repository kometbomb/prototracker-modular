#include "MixerModule.h"
#include <algorithm>
#include "SDL.h"

MixerModule::MixerModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 2, 2, 0)
{
}


MixerModule::~MixerModule()
{
}


void MixerModule::cycle()
{
	float mixing = getInput(1);
	float mixA, mixB;

	if (mixing < 0)
	{
		mixA = 1.0f;
		mixB = std::min(1.0f, std::max(0.0f, 1.0f - fabsf(mixing)));
	}
	else
	{
		mixA = std::min(1.0f, std::max(0.0f, 1.0f - mixing));
		mixB = 1.0f;
	}

	setOutput(0, mixA * getInput(0));
	setOutput(1, mixB * getInput(0));
}



const char * MixerModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Mixing"};
	return names[input];
}


const char * MixerModule::getOutputName(int output) const
{
	static const char *names[] = {"Output A", "Output B"};
	return names[output];
}


const char * MixerModule::getName() const
{
	return "Mixer";
}

SynthModule * MixerModule::createModule(ModularSynth& synth)
{
	return new MixerModule(synth);
}
