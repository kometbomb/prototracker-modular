#pragma once

#include "../SynthModule.h"

class NoiseModule: public SynthModule
{
	NoiseModule(ModularSynth& synth);

	unsigned long int mLfsrRegister, mRandomValue;
	float mAccumulator, mPreviousSync;

	void reset();
	void update();
	unsigned int nextRandomValue();

public:
	virtual ~NoiseModule();

	static const int moduleId = 18;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Noise";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
