#include "FilterModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include "SDL.h"

/*
The Huovilainen Moog ladder filter model comes from here:

https://github.com/ddiakopoulos/MoogLadders

Original implementation: Victor Lazzarini for CSound5
*/

FilterModule::FilterModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 2, 0), mCutoff(44100.0f), mResonance(0.0f), mThermal(0.000025)
{
	SDL_memset(mStage, 0, sizeof(mStage));
	SDL_memset(mDelay, 0, sizeof(mDelay));
	SDL_memset(mStageTanh, 0, sizeof(mStageTanh));
}


FilterModule::~FilterModule()
{
}


void FilterModule::cycle()
{
	// Cutoff in kHz
	float inCutoff = std::max(0.0f, getInput(1) * 1000.0f);
	float inResonance = std::max(0.0f, std::min(0.999f, getInput(2)));
	
	// Only update coeffs if needed
	
	if (mCutoff != inCutoff) 
	{
		// setCutoff() will set resonance so update mResonance here
		mResonance = inResonance;
		setCutoff(inCutoff);
	}
	else if (mResonance != inResonance) 
	{
		// Only update resonance
		setResonance(inResonance);
	}
	
	for (int j = 0; j < 2; j++) 
	{
		float input = getInput(0) - mResQuad * mDelay[5];
		mDelay[0] = mStage[0] = mDelay[0] + mTune * (tanh(input * mThermal) - mStageTanh[0]);
		
		for (int k = 1; k < 4; k++) 
		{
			input = mStage[k-1];
			mStage[k] = mDelay[k] + mTune * ((mStageTanh[k-1] = tanh(input * mThermal)) - (k != 3 ? mStageTanh[k] : tanh(mDelay[k] * mThermal)));
			mDelay[k] = mStage[k];
		}
		
		// 0.5 sample mDelay for phase compensation
		mDelay[5] = (mStage[3] + mDelay[4]) * 0.5;
		mDelay[4] = mStage[3];
	}
	
	// Lowpass out
	setOutput(0, mDelay[5]);
	
	// Hipass out
	setOutput(1, getInput(0) - mDelay[5]);
}


void FilterModule::setResonance(float r)
{
	mResonance = r;
	mResQuad = 4.0 * mResonance * mAcr;
}


void FilterModule::setCutoff(float c)
{
	mCutoff = c;

	float fc =  mCutoff / mSampleRate;
	float f  =  fc * 0.5;
	float fc2 = fc * fc;
	float fc3 = fc * fc * fc;
	float fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
	
	mAcr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;
	mTune = (1.0 - exp(-((2 * M_PI) * f * fcr))) / mThermal; 

	setResonance(mResonance);
}


const char * FilterModule::getInputName(int input) const 
{
	static const char *names[] = {"Input", "Cutoff", "Resonance"};
	return names[input];
}


const char * FilterModule::getOutputName(int output) const 
{
	static const char *names[] = {"LowOut", "HiOut"};
	return names[output];
}


const char * FilterModule::getName() const
{
	return "Filter";
}

SynthModule * FilterModule::createModule(ModularSynth& synth)
{
	return new FilterModule(synth);
}


void FilterModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);
	setCutoff(mCutoff);
}
