#pragma once

#include "Editor.h"
#include "SDL.h"
#include "ModularSynth.h"
#include "PathFinder.h"

class ISynth;
struct Color;
class ModuleSelector;

class SynthGrid: public Editor
{
public:
	static const int gridWidth = 4;
	static const int gridHeight = ModularSynth::maxModules / gridWidth;
	static const int gridResolution = 8;
	static const int moduleMargin = 1;
	static const int networkWidth = gridWidth * gridResolution + 1;
	static const int networkHeight = gridHeight * gridResolution + 1;
private:
	ISynth& mSynth;
	ModuleSelector *mModuleSelector;
	
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	void drawWire(Renderer& renderer, int x1, int y1, int x2, int y2, const Color& color1, const Color& color2) const;
	void drawAngledWire(Renderer& renderer, int x1, int y1, int x2, int y2, int y3, const Color& color1) const;
	SDL_Rect getModuleArea(int index, const SDL_Rect& parent, bool shrinkByMargin = false) const; 
	
	// type: 0 = input, 1 = output
	SDL_Rect getConnectorArea(int moduleIndex, int type, int connectorIndex, const SDL_Rect& parent) const;
	int getConnectorNode(int moduleIndex, int type, int connectorIndex) const;
	
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
	
	int mModuleLocation[gridWidth * gridHeight][2];
	std::vector<PathFinder::Node> mNetwork;
	std::vector<std::vector<SDL_Point> > mConnectionPath;
	
	void initNetwork();
	void rebuildWires();
	
	ModularSynth& getModularSynth();
	const ModularSynth& getModularSynth() const;
	
	bool pickConnector(int x, int y, const SDL_Rect& area, int& module, int& connectorType, int& connector);
	bool pickModule(int x, int y, const SDL_Rect& area, int& module, bool includeFree = false);
	
	void showNewModuleDialog();
	
	void startMove(int module);
	void endMove(int targetModule);
	void startConnect(int fromModule, int toModule, int fromOutput, int toInput);
	void endConnect(int module, int connector);
	
public:
	
	SynthGrid(EditorState& editorState, ISynth& synth);
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onFileSelectorEvent(const Editor& moduleSelector, bool accept);
	virtual void onListenableChange(Listenable *listenable);
	virtual void onLoaded();
};
