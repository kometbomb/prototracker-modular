#pragma once

class SynthModule
{
public:
	typedef SynthModule* (*CreateModuleFunc)(void); 

	static const int maxInputs = 8;
	static const int maxOutputs = 8;
	
protected:
	const int mSynthId;
	int mNumInputs, mNumOutputs;
	float mInputs[maxInputs], mOutputs[maxOutputs];

	void setOutput(int output, float value);
	SynthModule(int synthId, int numInputs, int numOutputs);
	
public:
	
	virtual ~SynthModule();
	
	int getSynthId() const;
	
	void setInput(int input, float value);
	float getInput(int input) const;
	float getOutput(int output) const;
	
	int getNumInputs() const;
	int getNumOutputs() const;
	
	virtual const char * getName() const = 0;
	virtual const char * getInputName(int input) const = 0;
	virtual const char * getOutputName(int output) const = 0;
	
	virtual void cycle() = 0;
};
