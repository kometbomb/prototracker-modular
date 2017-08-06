#pragma once

#include "SynthModule.h"

class OscillatorModule: public SynthModule
{
	float mAccumulator;
public:
	OscillatorModule();
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

