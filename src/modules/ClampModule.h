#pragma once

#include "../SynthModule.h"

class ClampModule: public SynthModule
{
	ClampModule(ModularSynth& synth);

public:
	static const ModuleClass moduleClass = ModuleClass::Modifier;
	static const int moduleId = 10;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Clamp";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
