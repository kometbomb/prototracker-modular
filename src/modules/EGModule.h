#pragma once

#include "../SynthModule.h"

class EGModule: public SynthModule
{
	enum { Idle, Attack, Full, Decay } mState;
	float mAmp;
	bool mPrevKeyOn;

	EGModule(ModularSynth& synth);

public:
	static const ModuleClass moduleClass = ModuleClass::Control;
	static const int moduleId = 14;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "EG";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
