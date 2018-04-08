#include "VirtualModule.h"
#include "../ModularSynth.h"
#include "ExtInModule.h"
#include "ExtOutModule.h"
#include "../SynthGrid.h"
#include "../Debug.h"
#include <cstdio>

VirtualModule::VirtualModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0, true), mCurrentActiveTrack(0), mPrevKeyonValue(0)
{
	for (int i = 0 ; i < maxVirtualTracks ; ++i)
	{
		mContainerSynth[i] = synth.createEmpty(false);
		mContainerSynth[i]->setName(moduleName);
	}

	mContainerSynth[0]->addChangeListener(this);
}


VirtualModule::~VirtualModule()
{
	for (int i = 0 ; i < maxVirtualTracks ; ++i)
		delete mContainerSynth[i];
}


void VirtualModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);
	for (int i = 0 ; i < maxVirtualTracks ; ++i)
		mContainerSynth[i]->setSampleRate(newRate);
}


void VirtualModule::cycle()
{
	float keyOn = getInput(0);
	if (keyOn > 0.5f && mPrevKeyonValue <= 0.5f)
	{
		gotoNextTrack();

		// Note triggered, notify currently queued track
		getCurrentActiveTrack().triggerNote();
	}

	mPrevKeyonValue = keyOn;

	// Only update the active synth inputs

	for (int index = 0 ; index < ModularSynth::numExtConnections ; ++index)
		getCurrentActiveTrack().setExtInput(index, getInput(index + 1));

	for (int i = 0 ; i < maxVirtualTracks ; ++i)
		mContainerSynth[i]->cycle();

	// Output is a sum of all outputs of virtual tracks

	for (int i = 0 ; i < maxOutputs ; ++i)
		mOutputs[i] = 0;

	for (int track = 0 ; track < maxVirtualTracks ; ++track)
	{
		for (int index = 0 ; index < ModularSynth::numExtConnections ; ++index)
			setOutput(index, mOutputs[index] + mContainerSynth[track]->getExtOutput(index));
	}
}


const char * VirtualModule::getInputName(int input) const
{
	if (input == 0)
		return "KeyOn";

	static char name[50];
	snprintf(name, sizeof(name), "ExtIn%d", input - 1);
	return name;
}


const char * VirtualModule::getOutputName(int output) const
{
	static char name[50];
	snprintf(name, sizeof(name), "ExtOut%d", output);
	return name;
}


const char * VirtualModule::getName() const
{
	return mContainerSynth[0]->getName();
}


SynthModule * VirtualModule::createModule(ModularSynth& synth)
{
	return new VirtualModule(synth);
}


bool VirtualModule::onDataLoad(const FileSection& section)
{
	int offset = 0;
	return mContainerSynth[0]->readSynth(section, offset);
}


void VirtualModule::onDataSave(FileSection& section)
{
	mContainerSynth[0]->writeSynth(section);
}


void VirtualModule::onAction(SynthGrid& synthGrid)
{
	synthGrid.setModularSynth(*mContainerSynth[0], true);
}


void VirtualModule::copy(const SynthModule& source)
{
	SynthModule::copy(source);
	mContainerSynth[0]->copy(static_cast<const VirtualModule&>(source).getModularSynth());
}


const ModularSynth& VirtualModule::getModularSynth() const
{
	return *mContainerSynth[0];
}


void VirtualModule::updateExtConnectionCounts()
{
	mNumInputs = 0;
	mNumOutputs = 0;

	// Find the highest param 0 for ExtIn/Out modules which tells the number of
	// inputs/outputs to display

	for (int i = 0 ; i < ModularSynth::maxModules ; ++i)
	{
		const SynthModule *module = mContainerSynth[0]->getModule(i);

		if (module != NULL)
		{
			if (module->getSynthId() == ExtInModule::moduleId)
				mNumInputs = std::max(mNumInputs, static_cast<int>(module->getParam(0)) + 1);
			else if (module->getSynthId() == ExtOutModule::moduleId)
				mNumOutputs = std::max(mNumOutputs, static_cast<int>(module->getParam(0)) + 1);
		}
	}

	// The first input is the trigger input so we need one more

	mNumInputs++;
}


void VirtualModule::onLoaded()
{
	updateExtConnectionCounts();
}


void VirtualModule::onShow()
{
	updateExtConnectionCounts();
}


ModularSynth& VirtualModule::getCurrentActiveTrack()
{
	return *mContainerSynth[mCurrentActiveTrack];
}


void VirtualModule::gotoNextTrack()
{
	mCurrentActiveTrack = (mCurrentActiveTrack + 1) % maxVirtualTracks;
	debug("Active track = %d", mCurrentActiveTrack);
}


void VirtualModule::cloneTracks()
{
	debug("Cloning tracks");

	// Clone the first synth

	for (int i = 1 ; i < maxVirtualTracks ; ++i)
		mContainerSynth[i]->copy(*mContainerSynth[0]);
}


void VirtualModule::onListenableChange(Listenable *listenable)
{
	cloneTracks();
}
