#pragma once

#include "../SynthModule.h"

class AddModule: public SynthModule
{
	AddModule(ModularSynth& synth);

public:
	static const ModuleClass moduleClass = ModuleClass::Arithmetic;
	static const int moduleId = 5;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Add";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
