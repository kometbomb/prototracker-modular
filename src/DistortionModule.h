#pragma once

#include "SynthModule.h"

class DistortionModule: public SynthModule
{
	DistortionModule(ModularSynth& synth);
	
public:
	virtual ~DistortionModule();

	static const int moduleId = 17;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Distortion";

	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};

