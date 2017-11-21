#pragma once

#include "SDL.h"
#include "Color.h"

struct ModularSynth;
struct Renderer;

class SynthModule
{
public:
	typedef SynthModule* (*CreateModuleFunc)(ModularSynth&);

	static const int maxInputs = 8;
	static const int maxOutputs = 8;
	static const int maxParams = 8;

	enum ModuleClass
	{
		Unknown,
		Source,
		Generator,
		Modifier,
		Control,
		Arithmetic,
		NumClasses
	};

protected:
	ModularSynth& mSynth;
	const int mSynthId;
	const ModuleClass mModuleClass;
	int mNumInputs, mNumOutputs, mNumParams;
	float mInputs[maxInputs], mOutputs[maxOutputs], mParams[maxParams];
	int mSampleRate;

	void setOutput(int output, float value);
	SynthModule(ModularSynth& synth, ModuleClass moduleClass, int synthId, int numInputs, int numOutputs, int numParams);

public:

	static const int maxInstances = -1;

	static Color getClassColor(ModuleClass moduleClass);

	virtual ~SynthModule();

	virtual void setSampleRate(int newRate);

	int getSynthId() const;

	void setInput(int input, float value);
	void setParam(int param, float value);
	float getInput(int input) const;
	float getOutput(int output) const;
	float getParam(int param) const;

	int getNumInputs() const;
	int getNumOutputs() const;
	int getNumParams() const;

	virtual void onLoaded();
	virtual void onDial(int delta);

	virtual const char * getName() const = 0;
	virtual const char * getInputName(int input) const = 0;
	virtual const char * getOutputName(int output) const = 0;

	// Return module area (used in wires pathfinding to find route around the module)
	virtual SDL_Rect getModuleArea(const SDL_Rect& gridCellArea) const;

	// Optional custom render methods
	virtual void render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const;
	virtual SDL_Rect getConnectorArea(const SDL_Rect& moduleArea, int type, int connectorIndex) const;
	virtual Color getModuleColor(bool isSelected) const;

	virtual void cycle() = 0;
};
