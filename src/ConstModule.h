#pragma once

#include "SynthModule.h"

class ConstModule: public SynthModule
{
	ConstModule(ModularSynth& synth);
	
public:
	static const int moduleId = 7;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Const";

	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

