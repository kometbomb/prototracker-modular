#pragma once

#include "SynthModule.h"

class PassthruModule: public SynthModule
{
	PassthruModule();
	
public:
	static const int moduleId = 2;
	static constexpr const char *moduleName = "Passthru";

	static SynthModule * createModule();
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

