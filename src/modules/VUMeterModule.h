#pragma once

#include "../SynthModule.h"

class VUMeterModule: public SynthModule
{
	VUMeterModule(ModularSynth& synth);

	float mInputValue;
	int mType;

public:
	static const int moduleId = 31;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "VUMeter";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();
	virtual void render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const;
	virtual void onDial(int delta);
	virtual void onLoaded();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;
};
