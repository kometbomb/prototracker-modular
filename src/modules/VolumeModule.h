#pragma once

#include "../SynthModule.h"

class VolumeModule: public SynthModule
{
	VolumeModule(ModularSynth& synth);

public:
	static const int moduleId = 41;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Volume";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
