#pragma once

#include "../SynthModule.h"

class DelayModule: public SynthModule
{
	DelayModule(ModularSynth& synth);
	int mMaxBufferSize;
	int mHead;
	float *mBuffer;

public:
	virtual ~DelayModule();

	static const ModuleClass moduleClass = ModuleClass::Modifier;
	static const int maxBufferSizeMs = 2000;

	static const int moduleId = 15;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Delay";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void setSampleRate(int newRate);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

};
