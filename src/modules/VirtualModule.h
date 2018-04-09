#pragma once

#include "../SynthModule.h"
#include "../Listener.h"

class VirtualModule: public SynthModule, public Listener
{
	VirtualModule(ModularSynth& synth);
	virtual ~VirtualModule();

public:
	static const int maxVirtualTracks = 4;

	ModularSynth *mContainerSynth[maxVirtualTracks];
	int mCurrentActiveTrack;
	float mPrevKeyonValue;

	void gotoNextTrack();
	void gotoNextFreeTrack();
	ModularSynth& getCurrentActiveTrack();
	void updateExtConnectionCounts();
	void cloneTracks();

protected:
	const ModularSynth& getModularSynth() const;

public:
	static const int moduleId = 35;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Virtual";

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

	virtual void onListenableChange(Listenable *listenable);
};
