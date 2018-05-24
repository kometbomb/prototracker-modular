#pragma once

#include "../SynthModule.h"

class OscilloscopeModule: public SynthModule
{
	OscilloscopeModule(ModularSynth& synth);
	float *mBuffer;
	float mPrevInput, mPrevSync;
	int mHead, mBufferLength, mSampleLength,
		mSampleStart, mSampleEnd, mLastUpdated;

	void initBuffer();

public:
	~OscilloscopeModule();

	static const int bufferLengthMs = 200;
	static const int moduleId = 29;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Oscilloscope";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();
	virtual void render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const;
	virtual void setSampleRate(int newRate);

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
