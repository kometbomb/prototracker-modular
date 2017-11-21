#pragma once

#include "../SynthModule.h"

class ShapeModule: public SynthModule
{
	ShapeModule(ModularSynth& synth);

public:
	static const ModuleClass moduleClass = ModuleClass::Modifier;
	static const int moduleId = 12;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Shape";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
