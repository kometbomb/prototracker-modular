#pragma once

#include "SynthModule.h"

class BitsModule: public SynthModule
{
	BitsModule(ModularSynth& synth);
	int mMaxBufferSize;
	int mHead;
	float *mBuffer;
	
public:
	virtual ~BitsModule();

	static const int moduleId = 20;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Bits";

	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

