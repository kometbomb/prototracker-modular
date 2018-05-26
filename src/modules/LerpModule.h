#pragma once

#include "../SynthModule.h"

class LerpModule: public SynthModule
{
	LerpModule(ModularSynth& synth);

public:
	static const int moduleId = 37;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Lerp";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
