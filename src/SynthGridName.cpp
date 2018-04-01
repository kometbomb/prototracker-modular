#include "SynthGridName.h"
#include "SynthGrid.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"
#include "ModularSynth.h"


SynthGridName::SynthGridName(EditorState& editorState, SynthGrid& synthGrid)
	: TextEditor(editorState), mSynthGrid(synthGrid)
{
	setSolidBackground(false);
	synthGrid.addListener(this);
}


SynthGridName::~SynthGridName()
{
}


void SynthGridName::onListenableChange(Listenable *listenable)
{
	setBuffer(mSynthGrid.getModularSynth().getName(), ModularSynth::nameLength);
}
