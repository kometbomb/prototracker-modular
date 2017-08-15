#pragma once

#include "SynthModule.h"

class AccumulatorModule: public SynthModule
{
	AccumulatorModule(ModularSynth& synth);
	float mAccumulator, mPreviousSync;
	
public:
	static const int moduleId = 11;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Accumulator";

	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

