#include "EffectModule.h"
#include "../ModularSynth.h"
#include <cstdio>
#include <cstring>

EffectModule::EffectModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 2, 1)
{
	setParam(0, '0');
}


void EffectModule::cycle()
{
	int value = mSynth.getEffectValue(static_cast<int>(mParams[0]) % 256);
	setOutput(0, static_cast<float>(value) / 64);
	setOutput(1, value);
}



const char * EffectModule::getInputName(int input) const
{
	return "";
}


const char * EffectModule::getOutputName(int output) const
{
	static const char *names[] = {"Norm. value", "Abs. value"};
	return names[output];
}


const char * EffectModule::getName() const
{
	return mLabel;
}


SynthModule * EffectModule::createModule(ModularSynth& synth)
{
	return new EffectModule(synth);
}


void EffectModule::onDial(int delta)
{
	const char *allowedEffects = "0123456789abcdefghijklmnopqrstuvwxyz";
	int effect = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + 256) % 256;
	int direction = delta < 0 ? -1 : 1;

	while (strchr(allowedEffects, effect) == NULL || effect == 0)
	{
		effect = (effect + direction + 256) % 256;
	}

	setParam(0, effect);
	sprintf(mLabel, "%cxx", effect);
}


void EffectModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}

void EffectModule::onShow()
{
	// Trigger mLabel update
	onDial(0);
}
