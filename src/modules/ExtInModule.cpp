#include "ExtInModule.h"
#include "../ModularSynth.h"
#include <cstdio>

ExtInModule::ExtInModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 1, 1)
{
	setParam(0, 0.0f);
}


void ExtInModule::cycle()
{
	int index = static_cast<int>(mParams[0]) % ModularSynth::numExtConnections;
	setOutput(0, mSynth.getExtInput(index));
}


const char * ExtInModule::getInputName(int input) const
{
	return NULL;
}


const char * ExtInModule::getOutputName(int output) const
{
	static const char *names[] = {"ExtIn"};
	return names[output];
}


const char * ExtInModule::getName() const
{
	return mLabel;
}


SynthModule * ExtInModule::createModule(ModularSynth& synth)
{
	return new ExtInModule(synth);
}


void ExtInModule::onDial(int delta)
{
	int index = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + ModularSynth::numExtConnections) % ModularSynth::numExtConnections;
	setParam(0, index);
	sprintf(mLabel, "%s%d", moduleName, index);
}


void ExtInModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}
