#pragma once

#include "../SynthModule.h"

class AutomationModule: public SynthModule
{
	AutomationModule(ModularSynth& synth);
	float mValue;
	char mLabel[50];

public:
	virtual ~AutomationModule();

	static const int moduleId = 22;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Automation";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();
	virtual void onLoaded();
	virtual void onDial(int delta);

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
