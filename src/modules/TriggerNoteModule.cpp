#include "TriggerNoteModule.h"
#include "../ModularSynth.h"

TriggerNoteModule::TriggerNoteModule(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 0, 1, 0)
{
}

void TriggerNoteModule::cycle()
{
	setOutput(0, mSynth.getNoteTrigger() ? 1.0f : 0.0f);
}



const char * TriggerNoteModule::getInputName(int input) const
{
	return "TriggerNote";
}


const char * TriggerNoteModule::getOutputName(int output) const
{
	return "Output";
}


const char * TriggerNoteModule::getName() const
{
	return "TriggerNote";
}

SynthModule * TriggerNoteModule::createModule(ModularSynth& synth)
{
	return new TriggerNoteModule(synth);
}
