#pragma once

#include "../SynthModule.h"

/*
 * Please note that this filter module is a placeholder
 * and will be most likely replaced or at least modified.
 */

class FilterModule: public SynthModule
{
	FilterModule(ModularSynth& synth);
	float mCutoff, mResonance;
	float mAcr, mResQuad, mTune, mThermal;
	float mStage[4];
	float mStageTanh[3];
	float mDelay[6];

	void setResonance(float r);
	void setCutoff(float c);

public:
	virtual ~FilterModule();

	static const int moduleId = 16;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Filter";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void setSampleRate(int newRate);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
