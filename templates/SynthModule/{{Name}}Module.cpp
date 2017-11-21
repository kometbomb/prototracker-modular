#include "{{Name}}Module.h"
#include <cstdio>

{{Name}}Module::{{Name}}Module(ModularSynth& synth)
	:SynthModule(synth, moduleClass, moduleId, 1, 1, 0)
{
}


void {{Name}}Module::cycle()
{
	// Passthru
	setOutput(0, getInput(0));
}



const char * {{Name}}Module::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * {{Name}}Module::getOutputName(int output) const
{
	static const char *names[] = {"Output"};
	return names[output];
}


const char * {{Name}}Module::getName() const
{
	return "{{Name}}";
}


SynthModule * {{Name}}Module::createModule(ModularSynth& synth)
{
	return new {{Name}}Module(synth);
}
