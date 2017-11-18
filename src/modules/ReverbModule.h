#pragma once

#include "../SynthModule.h"

class ReverbModule: public SynthModule
{
	static const int numTaps = 8;

	struct Tap {
		float delay, gain;
	};

	ReverbModule(ModularSynth& synth);
	Tap mTap[numTaps];
	int mHead, mMaxBufferSize;
	float *mBuffer;

public:
	virtual ~ReverbModule();

	static const int moduleId = 25;
	static const int maxInstances = -1;
	static const int maxBufferSizeMs = 2000;
	static constexpr const char *moduleName = "Reverb";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

	virtual void setSampleRate(int newRate);
};
