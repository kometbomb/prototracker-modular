#include "AutomationTrackInfo.h"
#include "Renderer.h"
#include "Color.h"
#include "EditorState.h"


AutomationTrackInfo::AutomationTrackInfo(EditorState& editorState)
	: Editor(editorState, false)
{
	mEditorState.automationTrack.addListener(this);
}


AutomationTrackInfo::~AutomationTrackInfo()
{
}


void AutomationTrackInfo::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);
	renderer.renderTextV(area, Color(), "%02d", static_cast<int>(mEditorState.automationTrack));
}
