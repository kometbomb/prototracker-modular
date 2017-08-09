#pragma once

struct ModularSynth;

class SynthModule
{
public:
	typedef SynthModule* (*CreateModuleFunc)(ModularSynth&); 

	static const int maxInputs = 8;
	static const int maxOutputs = 8;
	static const int maxParams = 8;
	
protected:
	ModularSynth& mSynth;
	const int mSynthId;
	int mNumInputs, mNumOutputs, mNumParams;
	float mInputs[maxInputs], mOutputs[maxOutputs], mParams[maxParams];

	void setOutput(int output, float value);
	SynthModule(ModularSynth& synth, int synthId, int numInputs, int numOutputs, int numParams);
	
public:

	static const int maxInstances = -1;
	
	virtual ~SynthModule();
	
	int getSynthId() const;
	
	void setInput(int input, float value);
	void setParam(int param, float value);
	float getInput(int input) const;
	float getOutput(int output) const;
	float getParam(int param) const;
	
	int getNumInputs() const;
	int getNumOutputs() const;
	int getNumParams() const;
	
	virtual const char * getName() const = 0;
	virtual const char * getInputName(int input) const = 0;
	virtual const char * getOutputName(int output) const = 0;
	
	virtual void cycle() = 0;
};
