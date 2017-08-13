#include "SynthModule.h"

SynthModule::SynthModule(ModularSynth& synth, int synthId, int numInputs, int numOutputs, int numParams)
	: mSynth(synth), mSynthId(synthId), mNumInputs(numInputs), mNumOutputs(numOutputs), mNumParams(numParams)
{
	for (int i = 0 ; i < mNumInputs ; ++i)
		mInputs[i] = 0.0f;
	
	for (int i = 0 ; i < mNumOutputs ; ++i)
		mOutputs[i] = 0.0f;
	
	for (int i = 0 ; i < mNumParams ; ++i)
		mParams[i] = 0.0f;
}


SynthModule::~SynthModule()
{
}
	
	
int SynthModule::getSynthId() const
{
	return mSynthId;
}


void SynthModule::setParam(int param, float value)
{
	mParams[param] = value;
}

	
void SynthModule::setInput(int input, float value)
{
	mInputs[input] = value;
}


void SynthModule::setOutput(int output, float value)
{
	mOutputs[output] = value;
}


float SynthModule::getParam(int param) const
{
	return mParams[param];
}


float SynthModule::getInput(int input) const
{
	return mInputs[input];
}


float SynthModule::getOutput(int output) const
{
	return mOutputs[output];
}


int SynthModule::getNumInputs() const
{
	return mNumInputs;
}


int SynthModule::getNumOutputs() const
{
	return mNumOutputs;
}


int SynthModule::getNumParams() const
{
	return mNumParams;
}
	

void SynthModule::onDial(int delta)
{
}


void SynthModule::onLoaded()
{
}
	