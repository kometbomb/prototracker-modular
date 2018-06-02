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
}


SynthModule * DisplayModule::createModule(ModularSynth& synth)
{
	return new DisplayModule(synth);
}


void DisplayModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));
	SDL_Rect sign = {moduleArea.x + 3, moduleArea.y + 4, 5, 1};
	SDL_Rect textArea = {moduleArea.x - 10, moduleArea.y + moduleArea.h / 2 - 4, 100, 100};

	
	if (getInput(0) >= 0)
	{
		renderer.renderTextV(textArea, Color(255,255,255), "%+06.2f", getInput(0));
	}
	else if (getInput(0) < 0)
	{
		renderer.renderRect(sign, Color(255,255,255));
		renderer.renderTextV(textArea, Color(170,170,170), "%+06.2f", getInput(0) * -1);
	}
}