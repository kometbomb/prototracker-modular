#include "SynthModule.h"

SynthModule::SynthModule(int synthId, int numInputs, int numOutputs)
	: mSynthId(synthId), mNumInputs(numInputs), mNumOutputs(numOutputs)
{
}


SynthModule::~SynthModule()
{
}
	
	
int SynthModule::getSynthId() const
{
	return mSynthId;
}
	
void SynthModule::setInput(int input, float value)
{
	mInputs[input] = value;
}


void SynthModule::setOutput(int output, float value)
{
	mOutputs[output] = value;
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
	
	