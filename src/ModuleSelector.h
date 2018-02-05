#pragma once

#include "GenericSelector.h"
#include "ModuleFactory.h"

struct Label;
struct ModularSynth;

class ModuleSelector: public GenericSelector
{
	struct ModuleInfo: public Item {
		int id;
		const char *name;

		ModuleInfo(int _id, const char *_name): id(_id), name(_name) {}
	};

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);
	virtual void onSelectItem(const Item& item);

public:
	ModuleSelector(EditorState& editorState);
	virtual ~ModuleSelector();

	void populate(const ModularSynth& modularSynth);

	int getSelectedModuleId() const;
};
