#pragma once

#include "../SynthModule.h"

class ContainerModule: public SynthModule
{
	ContainerModule(ModularSynth& synth);
	~ContainerModule();

	ModularSynth *mContainerSynth;

	void updateExtConnectionCounts();

protected:
	const ModularSynth& getModularSynth() const;

public:
	static const int moduleId = 32;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Container";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void setSampleRate(int newRate);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

	virtual void onAction(SynthGrid& synthGrid);

	virtual bool onDataLoad(const FileSection& section);
	virtual void onDataSave(FileSection& section);
	virtual void onLoaded();
	virtual void onShow();

	virtual void copy(const SynthModule& source);
};
