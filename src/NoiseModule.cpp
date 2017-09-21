#include "NoiseModule.h"
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include "SDL.h"

NoiseModule::NoiseModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 2, 3, 0), mAccumulator(0), mPreviousSync(0)
{
	reset();
}


NoiseModule::~NoiseModule()
{
}


void NoiseModule::reset()
{
	mLfsrRegister = 1;
}


void NoiseModule::update()
{
	typedef unsigned int T;
	const T zero = (T)(0);
	const T lsb = zero + (T)(1);
	const T feedback = (
		(lsb << (22)) ^
		(lsb << (17))
	);
	mLfsrRegister = (mLfsrRegister >> 1) ^ ((zero - (mLfsrRegister & lsb)) & feedback);
}
	

void NoiseModule::cycle()
{
	if (getInput(1) > 0.0f && mPreviousSync <= 0.0f)
	{
		mAccumulator = 0;
		reset();
	}
	
	mPreviousSync = getInput(1);	
	
	mAccumulator += std::max(0.0f, getInput(0)) / mSampleRate * 10000.0f;
	
	while (mAccumulator >= 1.0f) 
	{
		mAccumulator -= 1.0f;
		update();
	}
	
	setOutput(0, static_cast<float>(mLfsrRegister & 0x0ffff) / 32768.0f - 1.0f);
	setOutput(1, static_cast<float>(mLfsrRegister & 0x0f000) / 32768.0f - 1.0f);
	setOutput(2, static_cast<float>(mLfsrRegister & 0x10000) / 32768.0f - 1.0f);
}


const char * NoiseModule::getInputName(int input) const 
{
	static const char *names[] = {"Frequency", "Sync"};
	return names[input];
}


const char * NoiseModule::getOutputName(int output) const 
{
	static const char *names[] = {"Output", "4-Bit", "1-Bit"};
	return names[output];
}


const char * NoiseModule::getName() const
{
	return "Noise";
}

SynthModule * NoiseModule::createModule(ModularSynth& synth)
{
	return new NoiseModule(synth);
}
