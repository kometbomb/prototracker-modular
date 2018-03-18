#pragma once

#include "Editor.h"

class AutomationTrackInfo: public Editor
{
public:
	AutomationTrackInfo(EditorState& editorState);
	virtual ~AutomationTrackInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
