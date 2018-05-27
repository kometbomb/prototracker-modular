#pragma once

#include "../SynthModule.h"

class IntModule: public SynthModule
{
	float mValue;
	char mLabel[50];
	
	IntModule(ModularSynth& synth);
	
public:
	static const int moduleId = 36;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Int";
	
	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual void onLoaded();
	virtual void onDial(int delta);
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};
