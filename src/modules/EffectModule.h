#pragma once

#include "../SynthModule.h"

class EffectModule: public SynthModule
{
	char mLabel[50];

	EffectModule(ModularSynth& synth);

public:
	static const int moduleId = 13;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Effect";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual void onLoaded();
	virtual void onDial(int delta);

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
