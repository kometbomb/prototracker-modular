#include "AutomationModule.h"
#include <cstdlib>
#include <cstdio>
#include "SDL.h"
#include "../Synth.h"
#include "../ModularSynth.h"

AutomationModule::AutomationModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 0, 1, 1)
{
}


AutomationModule::~AutomationModule()
{
}


void AutomationModule::cycle()
{
	setOutput(0, mSynth.getAutomationValue(mParams[0]));
}



const char * AutomationModule::getInputName(int input) const
{
	return "";
}


const char * AutomationModule::getOutputName(int output) const
{
	return "Output";
}


const char * AutomationModule::getName() const
{
	return mLabel;
}


SynthModule * AutomationModule::createModule(ModularSynth& synth)
{
	return new AutomationModule(synth);
}


void AutomationModule::onLoaded()
{
	// Trigger mLabel update
	onDial(0);
}


void AutomationModule::onDial(int delta)
{
	int track = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + Synth::maxAutomationTracks) % Synth::maxAutomationTracks;

	setParam(0, track);
	sprintf(mLabel, "Atm%d", track);
}
