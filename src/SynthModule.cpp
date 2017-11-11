#include "SynthModule.h"
#include "Renderer.h"
#include "Color.h"

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


void SynthModule::setSampleRate(int newRate)
{
	mSampleRate = newRate;
}


void SynthModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, isSelected ? Color(96,96,96) : Color(64,64,64));

	SDL_Rect textArea = {moduleArea.x + 2, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
	renderer.renderText(textArea, Color(255,255,255), getName());
}


SDL_Rect SynthModule::getConnectorArea(const SDL_Rect& moduleArea, int type, int connectorIndex) const
{
	const int connectorSize = 5;

	SDL_Rect area = {0,0,connectorSize,connectorSize};

	int c;

	if (type == 0)
		c = getNumInputs();
	else
		c = getNumOutputs();

	if (c > 0)
	{
		area.x = moduleArea.x + moduleArea.w * (connectorIndex * 2 + 1) / (c * 2) - connectorSize / 2;
		area.y = moduleArea.y;

		if (type == 1)
			area.y += moduleArea.h - area.h - 2;
		else
			area.y += 2;
	}

	return area;
}
