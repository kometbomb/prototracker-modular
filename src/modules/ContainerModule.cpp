#include "ContainerModule.h"
#include "../ModularSynth.h"
#include <cstdio>

ContainerModule::ContainerModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0, true)
{
	mContainerSynth = synth.createEmpty();
}


ContainerModule::~ContainerModule()
{
	delete mContainerSynth;
}


void ContainerModule::cycle()
{
	// Passthru
	setOutput(0, getInput(0));
}



const char * ContainerModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * ContainerModule::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * ContainerModule::getName() const
{
	return "Container";
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
