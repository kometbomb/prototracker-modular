#include "ModuleSelector.h"
#include "ModularSynth.h"
#include "Color.h"
#include "Renderer.h"
#include "Label.h"
#include <cstring>

ModuleSelector::ModuleSelector(EditorState& editorState)
	: GenericSelector(editorState)
{
	setTitle("Add module");
}


ModuleSelector::~ModuleSelector()
{
}


void ModuleSelector::populate(const ModularSynth& modularSynth)
{
	clearItems();

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

		addItem(new ModuleInfo(info.id, moduleFactory.getModuleInfo(i).name));
	}
}


void ModuleSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}


void ModuleSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void ModuleSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const GenericSelector::Item& item, bool isSelected)
{
	const ModuleInfo& moduleInfo = static_cast<const ModuleInfo&>(item);
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

	renderer.renderTextV(area, color, "%s", moduleInfo.name);
}


int ModuleSelector::getSelectedModuleId() const
{
	return static_cast<const ModuleInfo&>(getSelectedItem()).id;
}


void ModuleSelector::onSelectItem(const Item& item)
{
}
