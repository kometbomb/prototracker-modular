#pragma once

#include "../SynthModule.h"

class OscillatorModule: public SynthModule
{
	OscillatorModule(ModularSynth& synth);

	float mAccumulator, mPreviousSync;
public:
	static const ModuleClass moduleClass = ModuleClass::Generator;
	static const int moduleId = 1;
	static constexpr const char *moduleName = "Oscillator";

	static SynthModule * createModule(ModularSynth& synth);
	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
