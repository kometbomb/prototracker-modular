#pragma once

#include "TextEditor.h"
#include "Listener.h"

struct SynthGrid;

class SynthGridName: public TextEditor
{
	SynthGrid& mSynthGrid;

public:
	SynthGridName(EditorState& editorState, SynthGrid& synthGrid);
	virtual ~SynthGridName();

	// SynthGrid changed the edited synth
	virtual void onListenableChange(Listenable *listenable);
};
