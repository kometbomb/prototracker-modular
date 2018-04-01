#include "ContainerModule.h"
#include "../ModularSynth.h"
#include "ExtInModule.h"
#include "ExtOutModule.h"
#include "../SynthGrid.h"
#include <cstdio>

ContainerModule::ContainerModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0, true)
{
	mContainerSynth = synth.createEmpty();
	mContainerSynth->setName(moduleName);
}


ContainerModule::~ContainerModule()
{
	delete mContainerSynth;
}


void ContainerModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);
	mContainerSynth->setSampleRate(newRate);
}


void ContainerModule::cycle()
{
	for (int index = 0 ; index < ModularSynth::numExtConnections ; ++index)
		mContainerSynth->setExtInput(index, getInput(index));

	mContainerSynth->cycle();

	for (int index = 0 ; index < ModularSynth::numExtConnections ; ++index)
		setOutput(index, mContainerSynth->getExtOutput(index));
}


const char * ContainerModule::getInputName(int input) const
{
	static char name[50];
	snprintf(name, sizeof(name), "ExtIn%d", input);
	return name;
}


const char * ContainerModule::getOutputName(int output) const
{
	static char name[50];
	snprintf(name, sizeof(name), "ExtOut%d", output);
	return name;
}


const char * ContainerModule::getName() const
{
	return mContainerSynth->getName();
}


SynthModule * ContainerModule::createModule(ModularSynth& synth)
{
	return new ContainerModule(synth);
}


bool ContainerModule::onDataLoad(const FileSection& section)
{
	int offset = 0;
	return mContainerSynth->readSynth(section, offset);
}


void ContainerModule::onDataSave(FileSection& section)
{
	mContainerSynth->writeSynth(section);
}


void ContainerModule::onAction(SynthGrid& synthGrid)
{
	synthGrid.setModularSynth(*mContainerSynth);
}


void ContainerModule::copy(const SynthModule& source)
{
	SynthModule::copy(source);
	mContainerSynth->copy(static_cast<const ContainerModule&>(source).getModularSynth());
}


const ModularSynth& ContainerModule::getModularSynth() const
{
	return *mContainerSynth;
}


void ContainerModule::updateExtConnectionCounts()
{
	mNumInputs = 0;
	mNumOutputs = 0;

	// Find the highest param 0 for ExtIn/Out modules which tells the number of
	// inputs/outputs to display

	for (int i = 0 ; i < ModularSynth::maxModules ; ++i)
	{
		const SynthModule *module = mContainerSynth->getModule(i);

		if (module != NULL)
		{
			if (module->getSynthId() == ExtInModule::moduleId)
				mNumInputs = std::max(mNumInputs, static_cast<int>(module->getParam(0)) + 1);
			else if (module->getSynthId() == ExtOutModule::moduleId)
				mNumOutputs = std::max(mNumOutputs, static_cast<int>(module->getParam(0)) + 1);
		}
	}
}


void ContainerModule::onLoaded()
{
	updateExtConnectionCounts();
}


void ContainerModule::onShow()
{
	updateExtConnectionCounts();
}
