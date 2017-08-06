#pragma once

#include "Editor.h"
#include "SDL.h"
#include "ModularSynth.h"

class ISynth;
struct Color;
class ModuleSelector;

class SynthGrid: public Editor
{
	ISynth& mSynth;
	ModuleSelector *mModuleSelector;
	
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	void drawWire(Renderer& renderer, int x1, int y1, int x2, int y2, const Color& color) const;
	SDL_Rect getModuleArea(int index, const SDL_Rect& parent) const; 
	
	// type: 0 = input, 1 = output
	SDL_Rect getConnectorArea(int moduleIndex, int type, int connectorIndex, const SDL_Rect& parent) const;
	
	enum SynthGridMode
	{
		IDLE,
		CONNECTING_MODULE,
		MOVING_MODULE
	};
	
	SynthGridMode mMode;
	int mSelectedModule;
	int mFromModule, mFromOutput;
	int mToModule, mToInput;
	int mMouseX, mMouseY;
	
	bool pickConnector(int x, int y, const SDL_Rect& area, int& module, int& connectorType, int& connector);
	bool pickModule(int x, int y, const SDL_Rect& area, int& module, bool includeFree = false);
	
	void showNewModuleDialog();
	
	void startMove(int module);
	void endMove(int targetModule);
	void startConnect(int fromModule, int toModule, int fromOutput, int toInput);
	void endConnect(int module, int connector);
	
public:
	static const int gridWidth = 4;
	static const int gridHeight = ModularSynth::maxModules / gridWidth;

	SynthGrid(EditorState& editorState, ISynth& synth);
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onFileSelectorEvent(const Editor& moduleSelector, bool accept);
};
