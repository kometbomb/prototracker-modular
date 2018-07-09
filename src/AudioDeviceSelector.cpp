#include "AudioDeviceSelector.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include "MessageBox.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include "Mixer.h"

AudioDeviceSelector::AudioDeviceSelector(EditorState& editorState)
	: GenericSelector(editorState)
{

}


AudioDeviceSelector::~AudioDeviceSelector()
{
}


void AudioDeviceSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}


void AudioDeviceSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void AudioDeviceSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
	const AudioDeviceItem& deviceItem = static_cast<const AudioDeviceItem&>(item);
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

	int width = area.w / renderer.getFontWidth() - 10;

	renderer.renderTextV(area, color, "%s", deviceItem.name.c_str());
}


void AudioDeviceSelector::populate(const Mixer& mixer)
{
	invalidateParent();

	clearItems();

	int numDevices = mixer.getNumDevices();
	for (int index = 0 ; index < numDevices ; ++index)
		addItem(new AudioDeviceItem(mixer.getDevice(index)));

	selectItem(0);
}


const char * AudioDeviceSelector::getSelectedDevice() const
{
	return static_cast<const AudioDeviceItem&>(getSelectedItem()).name.c_str();
}


AudioDeviceSelector::AudioDeviceItem::AudioDeviceItem(const char *_name)
	: name(_name)
{
}
