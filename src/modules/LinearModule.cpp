#include "LinearModule.h"
#include <cstdio>
#include <algorithm>

LinearModule::LinearModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0)
{
}


void LinearModule::cycle()
{
	float input = getInput(0) * 0.5f + 0.5f;
	float x = std::min(1.0f, std::max(0.0f, getInput(1) * 0.5f + 0.5f));
	float y = std::min(1.0f, std::max(0.0f, getInput(2) * 0.5f + 0.5f));
	float amp;

	if (x >= 1.0f)
	{
		amp = y;
	}
	if (x == 0.0f)
	{
		amp = y;
	}
	else if (input >= x)
	{
		amp = (1.0f - y) * (input - x) / (1.0f - x) + y;
	}
	else
	{
		amp = y * input / x;
	}

	setOutput(0, amp * 2.0f - 1.0f);
}



const char * LinearModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "X", "Y"};
	return names[input];
}


const char * LinearModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * LinearModule::getName() const
{
	return "Linear";
}


SynthModule * LinearModule::createModule(ModularSynth& synth)
{
	return new LinearModule(synth);
}
