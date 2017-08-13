#include "ModuleSelector.h"
#include "ModularSynth.h"
#include "Color.h"
#include "Renderer.h"
#include "Label.h"
#include <cstring>

ModuleSelector::ModuleSelector(EditorState& editorState)
	: Editor(editorState), mSelectedItem(0)
{
	mLabel = new Label(editorState);
	mLabel->setColor(Color(0, 0, 0));
	mLabel->setBackground(Color(255, 255, 255));
	addChild(mLabel, 0, 0, 100, 8);
	setTitle("Add module");
}


ModuleSelector::~ModuleSelector()
{
	delete mLabel;
}


void ModuleSelector::populate(const ModularSynth& modularSynth)
{
	mSelectedItem = 0;
	mModules.clear();
	
	ModuleFactory moduleFactory;
	
	for (int i = 0 ; i < moduleFactory.getNumModules() ; ++i)
	{
		const ModuleFactory::Module& info = moduleFactory.getModuleInfo(i);
		
		// Check if max number of instances for this SynthModule type
		// are limited
		
		if (info.maxInstances != -1)
		{
			int count = 0;
			for (int i = 0 ; i < ModularSynth::maxModules ; ++i)
			{
				const SynthModule *module = modularSynth.getModule(i);
				if (module != NULL && module->getSynthId() == info.id)
					++count;
			}
				
			// Don't add in the list (should probably add but disabled)
			
			if (count >= info.maxInstances)
				continue;
		}
		
		mModules.push_back(ModuleInfo(info.id, moduleFactory.getModuleInfo(i).name));
	}
}
	

bool ModuleSelector::onEvent(SDL_Event& event)
{
	if (mModal)
		return mModal->onEvent(event);
	
	switch (event.type)
	{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				reject(true);
				return true;
			}
			
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				accept(false);
				return true;
			}
			
			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
					selectItem(mSelectedItem - 1);
					return true;
					
				case SDLK_DOWN:
					selectItem(mSelectedItem + 1);
					return true;
			}
			
			break;
	}
	
	return false;
}


void ModuleSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}

	
void ModuleSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void ModuleSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const ModuleInfo& item, bool isSelected)
{
	Color color;
	
	if (isSelected)
		color = Color(255, 0, 0);
	
	renderer.clearRect(area, Color(0, 0, 0));
	
	renderer.renderTextV(area, color, "%s", item.name);
}


void ModuleSelector::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
	{
		renderer.clearRect(area, Color(0, 0, 0));
	}
	
	int countVisible = (area.h - 8 - 8 - 8) / 8;
	int firstVisible = mSelectedItem - countVisible / 2;
	
	if (firstVisible < 0)
	{
		firstVisible = 0;
	}	
	
	int lastVisible = firstVisible + countVisible;
	
	if (lastVisible >= mModules.size())
	{
		lastVisible = mModules.size() - 1;
		firstVisible = lastVisible - countVisible;
		if (firstVisible < 0)
		{
			firstVisible = 0;
		}	
	}
	
	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - firstVisible) * 8 + area.y + 16, area.w, 8};
		renderItem(renderer, textArea, mModules[row], row == mSelectedItem);
	}
}


int ModuleSelector::getSelectedModuleId() const
{
	return mModules[mSelectedItem].id;
}


void ModuleSelector::selectItem(int index)
{
	mSelectedItem = index;
	
	if (mSelectedItem >= static_cast<int>(mModules.size()))
		mSelectedItem = static_cast<int>(mModules.size()) - 1;
		
	if (mSelectedItem < 0)
		mSelectedItem = 0;
		
	setDirty(true);
}


void ModuleSelector::setTitle(const char *title)
{
	strncpy(mTitle, title, titleSize);
	mLabel->setText(mTitle);
	setModal(NULL);
}
