#include "DisplayModule.h"
#include "../Renderer.h"
#include <cstdio>
#include <functional>

DisplayModule::DisplayModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0), floatInput(0)
{
}


void DisplayModule::cycle()
{
	floatInput = getInput(0);
	
	// Passthru
	setOutput(0, getInput(0));
}



const char * DisplayModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * DisplayModule::getOutputName(int output) const
{
	static const char *names[] = {"Passthru"};
	return names[output];
}


const char * DisplayModule::getName() const
{
	return "Display";
}

void DisplayModule::onLoaded()
{
	setInput(0, 0.0f);
	stringMeUp(getInput(0));
}


char * DisplayModule::stringMeUp(float value) const
{
	static char funky[20];
	sprintf(funky, "%.2f", value);
	
	return funky;
}


SynthModule * DisplayModule::createModule(ModularSynth& synth)
{
	return new DisplayModule(synth);
}


void DisplayModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));
	
	
	
	SDL_Rect textArea = {moduleArea.x + 2, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
	renderer.renderText(textArea, Color(255,255,255), stringMeUp(floatInput));
}


