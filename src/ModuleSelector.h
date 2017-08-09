#pragma once

#include <vector>
#include "Editor.h"
#include "ModuleFactory.h"

struct Label;

class ModuleSelector: public Editor
{
	struct ModuleInfo {
		int id;
		const char *name;
		
		ModuleInfo(int _id, const char *_name): id(_id), name(_name) {}
	};
	std::vector<ModuleInfo> mModules;
	int mSelectedItem;
	static const int titleSize = 256;
	char mTitle[titleSize];
	Label *mLabel;
	
	void renderItem(Renderer& renderer, const SDL_Rect& area, const ModuleInfo& item, bool isSelected);
	void accept(bool isFinal = false);
	void reject(bool isFinal = false);
	void selectItem(int index);
	
public:
	ModuleSelector(EditorState& editorState);
	virtual ~ModuleSelector();
	
	/* Set dialog title
	 */
	void setTitle(const char *title);
	
	int getSelectedModuleId() const;
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
