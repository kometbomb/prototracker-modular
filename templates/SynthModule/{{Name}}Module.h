#pragma once

#include "SynthModule.h"

class {{Name}}Module: public SynthModule
{
	{{Name}}Module(ModularSynth& synth);

public:
	static const int moduleId = {{moduleId}};
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "{{Name}}";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
