#pragma once

#include "../SynthModule.h"

class ContainerModule: public SynthModule
{
	ContainerModule(ModularSynth& synth);
	~ContainerModule();

	ModularSynth *mContainerSynth;

public:
	static const int moduleId = 32;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Container";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

	virtual bool onDataLoad(const FileSection& section);
	virtual void onDataSave(FileSection& section);

};
