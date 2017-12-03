#pragma once

#include "../SynthModule.h"

class GlideModule: public SynthModule
{
	float mValue;

	GlideModule(ModularSynth& synth);

public:
	static const int moduleId = 28;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Glide";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
