#pragma once

#include "../SynthModule.h"

class ValueModule: public SynthModule
{
	ValueModule(ModularSynth& synth);

	char mInputValue[50];
	int mType;

public:
//	static const ModuleClass moduleClass = ModuleClass::{{moduleClass}};
	static const int moduleId = 35;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Value";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void onLoaded();
	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
