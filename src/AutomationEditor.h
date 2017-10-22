#pragma once

#include "Editor.h"
#include "SDL.h"
#include "Synth.h"

class IPlayer;

class AutomationEditor: public Editor
{
private:
	ISynth& mSynth;
	int mSelectedNode, mSelectedTrack, mMouseX, mMouseY;
	
	int getScrollPosition() const;
	bool pickNode(int x, int y, const SDL_Rect& area, int& nodeOut) const;
	Synth& getSynth();
	Synth::AutomationTrack& getAutomationTrack();
	const Synth& getSynth() const;
	const Synth::AutomationTrack& getAutomationTrack() const;
	static int granularize(int value, int steps);
	SDL_Rect getNodeArea(const SDL_Rect& parent, int node) const;
	SDL_Rect getNodeArea(const SDL_Rect& parent, const Synth::AutomationNode& node) const;
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
public:
	
	AutomationEditor(EditorState& editorState, ISynth& synth, IPlayer& player);
	virtual ~AutomationEditor();
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onListenableChange(Listenable *listenable);
	virtual void onLoaded();
};
