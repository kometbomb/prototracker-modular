#pragma once

#include "../SynthModule.h"

class DisplayModule: public SynthModule
{
	DisplayModule(ModularSynth& synth);

public:
	static const int moduleId = 35;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Display";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();
	virtual void render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const;

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
