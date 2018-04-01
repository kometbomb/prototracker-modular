#pragma once

#include "Editor.h"
#include "SDL.h"
#include "ModularSynth.h"
#include "PathFinder.h"
#include "Listenable.h"
#include <stack>

class ISynth;
class IPlayer;
struct Color;
class ModuleSelector;

class SynthGrid: public Editor, public Listenable
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
	IPlayer& mPlayer;
	ModuleSelector *mModuleSelector;
	ModularSynth *mCurrentModularSynth;
	std::stack<ModularSynth*> mParentSynth;

	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	void drawAngledWire(Renderer& renderer, int x1, int y1, int x2, int y2, int y3, const Color& color1) const;
	void drawConnection(Renderer& renderer, const SDL_Rect& area, int index, const Color& color);
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
	int mHoveredConnection;
	int mFromModule, mFromOutput;
	int mToModule, mToInput;
	int mMouseX, mMouseY;
	ModularSynth *mCopyBuffer;

	int mModuleLocation[gridWidth * gridHeight][2];
	std::vector<PathFinder::Node> mNetwork;
	std::vector<std::vector<SDL_Point> > mConnectionPath;

	void initNetwork();
	void rebuildWires();

	int findConnectionFrom(int fromModule, int fromOutput) const;
	int findConnectionTo(int toModule, int toOutput) const;

	bool pickConnector(int x, int y, const SDL_Rect& area, int& module, int& connectorType, int& connector);
	bool pickModule(int x, int y, const SDL_Rect& area, int& module, bool includeFree = false);

	void showNewModuleDialog();

	void startMove(int module);
	void endMove(int targetModule);
	void startConnect(int fromModule, int toModule, int fromOutput, int toInput);
	void endConnect(int module, int connector);

	void copySynth();
	void pasteSynth();
	void gotoParentSynth();

public:

	SynthGrid(EditorState& editorState, ISynth& synth, IPlayer& player);
	virtual ~SynthGrid();

	void setModularSynth(ModularSynth& synth, bool rememberParent = false);
	ModularSynth& getModularSynth();
	const ModularSynth& getModularSynth() const;

	virtual bool onEvent(SDL_Event& event);
	virtual void onFileSelectorEvent(const Editor& moduleSelector, bool accept);
	virtual void onListenableChange(Listenable *listenable);
	virtual void onLoaded();
};
