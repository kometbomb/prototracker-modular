#pragma once

#include "SDL.h"
#include "Color.h"

struct ModularSynth;
struct Renderer;
struct FileSection;
struct SynthGrid;

class SynthModule
{
public:
	typedef SynthModule* (*CreateModuleFunc)(ModularSynth&);

	static const int maxInputs = 8;
	static const int maxOutputs = 8;
	static const int maxParams = 8;

protected:
	ModularSynth& mSynth;
	const int mSynthId;
	int mNumInputs, mNumOutputs;
	const int mNumParams;
	// If the loader should call onDataLoad()/onDataSave()
	const bool mHasData;
	float mInputs[maxInputs], mOutputs[maxOutputs], mParams[maxParams];
	int mSampleRate;

	void setOutput(int output, float value);
	SynthModule(ModularSynth& synth, int synthId, int numInputs, int numOutputs,
		int numParams, bool hasData = false);

public:

	static const int maxInstances = -1;

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

	virtual void copy(const SynthModule& source);

	// After synth has been created
	virtual void onLoaded();

	// When SynthGrid displays a new synth layout
	virtual void onShow();

	// Mousewheel
	virtual void onDial(int delta);

	// Doubleclick, grid that called is passed
	virtual void onAction(SynthGrid& synthGrid);

	// These get called when the module needs to save any internal data
	virtual bool onDataLoad(const FileSection& section);
	virtual void onDataSave(FileSection& section);

	bool getHasData() const;

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
