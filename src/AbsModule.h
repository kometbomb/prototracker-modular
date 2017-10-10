#pragma once

#include "SynthModule.h"

class AbsModule: public SynthModule
{
	AbsModule(ModularSynth& synth);
	int mMaxBufferSize;
	int mHead;
	float *mBuffer;
	
public:
	virtual ~AbsModule();

	static const int moduleId = 21;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Abs";

	static SynthModule * createModule(ModularSynth& synth);
	
	virtual void cycle();
	
	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
	
};

