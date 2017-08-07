#include "SynthModule.h"

SynthModule::SynthModule(int synthId, int numInputs, int numOutputs, int numParams)
	: mSynthId(synthId), mNumInputs(numInputs), mNumOutputs(numOutputs), mNumParams(numParams)
{
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
	
