#include "DisplayModule.h"
#include "../Renderer.h"
#include <cstdio>
#include "Debug.h"

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
	return "Display";
}


void DisplayModule::onLoaded()
{
	setInput(0, 0.0f);
	floatToChar(getInput(0));
}


SynthModule * DisplayModule::createModule(ModularSynth& synth)
{
	return new DisplayModule(synth);
}


void DisplayModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));
	
	if (getInput(0) >= 0)
	{
		SDL_Rect textArea = {moduleArea.x - 10, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
		renderer.renderText(textArea, Color(255,255,255), floatToChar(getInput(0)));
	}
	else if (getInput(0) < 0)
	{
		float input = getInput(0) * -1;
		
		SDL_Rect textArea = {moduleArea.x - 10, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};
		renderer.renderText(textArea, Color(170,170,170), floatToChar(input));
	}
	
}


char * DisplayModule::floatToChar(float value) const
{
	static char valueStr[25];
	
	snprintf(valueStr, 25, "%+06.2f", value);
	
	return valueStr;
}
