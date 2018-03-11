#include "VUMeterModule.h"
#include "../Renderer.h"
#include <cstdio>
#include <algorithm>

VUMeterModule::VUMeterModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 1, 1, 1), mInputValue(0), mType(0)
{
}


void VUMeterModule::cycle()
{
	mInputValue = getInput(0);

	// Passthru
	setOutput(0, getInput(0));
}



const char * VUMeterModule::getInputName(int input) const
{
	static const char *names[] = {"Input"};
	return names[input];
}


const char * VUMeterModule::getOutputName(int output) const
{
	static const char *names[] = {"Passthru"};
	return names[output];
}


const char * VUMeterModule::getName() const
{
	return "VUMeter";
}


SynthModule * VUMeterModule::createModule(ModularSynth& synth)
{
	return new VUMeterModule(synth);
}


void VUMeterModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));

	switch (mType)
	{
		// 0.0 .. 1.0
		default:
		case 0: {
			float input = std::min(1.0f, std::max(0.0f, mInputValue));
			SDL_Rect meterArea = { moduleArea.x, moduleArea.y,
				static_cast<int>(static_cast<float>(moduleArea.w) * input), moduleArea.h };

			renderer.renderRect(meterArea, Color(192, 192, 192));
		} break;

		// -1.0 .. 1.0
		case 1: {
			float input = std::min(1.0f, std::max(-1.0f, mInputValue));
			int x1 = static_cast<int>(static_cast<float>(moduleArea.w) * (input * 0.5f + 0.5f));
			int x2 = moduleArea.w / 2;
			SDL_Rect meterArea = { moduleArea.x + std::min(x1, x2), moduleArea.y,
				abs(x1 - x2), moduleArea.h };
			renderer.renderRect(meterArea, Color(192, 192, 192));
			renderer.renderLine(moduleArea.x + moduleArea.w / 2, moduleArea.y,
				moduleArea.x + moduleArea.w / 2, moduleArea.y + moduleArea.h - 1, Color());
		} break;
	}
}


void VUMeterModule::onDial(int delta)
{
	mType = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + 2) % 2;
	setParam(0, mType);
}


void VUMeterModule::onLoaded()
{
	// Trigger update
	onDial(0);
}
