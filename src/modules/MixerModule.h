#pragma once

#include "../SynthModule.h"

class MixerModule: public SynthModule
{
	MixerModule(ModularSynth& synth);

public:
	virtual ~MixerModule();

	static const int moduleId = 23;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Mixer";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
