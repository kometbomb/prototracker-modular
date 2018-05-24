#include "DisplayModule.h"
#include "../Renderer.h"
#include <cstdio>

DisplayModule::DisplayModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 0)
{
}


void DisplayModule::cycle()
{
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
	return strValue;
}

void DisplayModule::onLoaded()
{
	setInput(0, 0.0f);
}


void DisplayModule::stringMeUp(float value) const
{
	sprintf(strValue, "%.2f", value);
}


SynthModule * DisplayModule::createModule(ModularSynth& synth)
{
	return new DisplayModule(synth);
}


void DisplayModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));
	
	stringMeUp(getInput(0));
	
	SDL_Rect textArea = {moduleArea.x + 2, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
	renderer.renderText(textArea, Color(255,255,255), getName());
}


