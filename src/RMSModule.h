#pragma once

#include "SynthModule.h"

class RMSModule: public SynthModule
{
	RMSModule(ModularSynth& synth);
	float *mBuffer;
	int mBufferLength, mHead;
	float mSum;

	static const int bufferLengthMs = 20;

	void initBuffer();

public:
	virtual ~RMSModule();

	static const int moduleId = 24;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "RMS";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual void setSampleRate(int newRate);

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
