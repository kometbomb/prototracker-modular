#pragma once

#include "SynthModule.h"

class PassthruModule: public SynthModule
{
public:
	PassthruModule();
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

