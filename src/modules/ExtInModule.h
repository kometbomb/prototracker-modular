#pragma once

#include "../SynthModule.h"

class ExtInModule: public SynthModule
{
	ExtInModule(ModularSynth& synth);

	char mLabel[50];

public:
	static const int moduleId = 33;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "ExtIn";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();
	virtual void onLoaded();
	virtual void onDial(int delta);

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
