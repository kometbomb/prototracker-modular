#pragma once

#include "../SynthModule.h"

class PulseModule: public SynthModule
{
	PulseModule(ModularSynth& synth);

	float mAccumulator, mPreviousSync;

public:
	virtual ~PulseModule();

	static const ModuleClass moduleClass = ModuleClass::Generator;
	static const int moduleId = 19;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Pulse";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
