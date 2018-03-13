#include "AutomationEditor.h"
#include "SDL.h"
#include "ISynth.h"
#include "IPlayer.h"
#include "Renderer.h"
#include "ModularSynth.h"
#include "SynthModule.h"
#include "EditorState.h"
#include "ModuleSelector.h"
#include "Color.h"
#include "Synth.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>



AutomationEditor::AutomationEditor(EditorState& editorState, ISynth& synth, IPlayer& player)
	:Editor(editorState, true), mSynth(synth), mSelectedNode(-1), mSelectedTrack(0)
{
	mEditorState.patternEditor.currentRow.addListener(this);
	mEditorState.sequenceEditor.currentRow.addListener(this);
}


AutomationEditor::~AutomationEditor()
{
}


SDL_Rect AutomationEditor::getNodeArea(const SDL_Rect& parent, int index) const
{
	const Synth& synth = static_cast<const Synth&>(mSynth);
	const Synth::AutomationNode& node = getAutomationTrack().getNode(index);

	return getNodeArea(parent, node);
}


SDL_Rect AutomationEditor::getNodeArea(const SDL_Rect& parent, const Synth::AutomationNode& node) const
{
	const int nodeWidth = 5;
	const int nodeHeight = 5;

	SDL_Rect area = {
		node.time + parent.x - nodeWidth / 2 - getScrollPosition(),
		static_cast<int>((1.0f - node.value) * parent.h) + parent.y - nodeHeight / 2,
		nodeWidth, nodeHeight
	};

	return area;
}


void AutomationEditor::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	if (mModal != NULL)
	{
		return;
	}

	renderer.setClip(area);
	renderer.renderBackground(area);

	const Synth& synth = getSynth();
	int patternLength = synth.getPatternLength();

	for (int x = area.x - getScrollPosition() ; x < area.w + area.x ; x += patternLength)
	{
		renderer.renderLine(x, area.y, x, area.y + area.h, Color(64,64,64));
	}

	float time = synth.getSongPosition();
	int editPos = mEditorState.patternEditor.currentRow + mEditorState.sequenceEditor.currentRow * patternLength;

	renderer.renderLine(time - getScrollPosition() + area.x, area.y, time - getScrollPosition() + area.x, area.y + area.h, Color(0,0,128));
	renderer.renderLine(editPos - getScrollPosition() + area.x, area.y, editPos - getScrollPosition() + area.x, area.y + area.h, Color(255,255,255));

	for (int track = 0 ; track < Synth::maxAutomationTracks ; ++track)
	{
		if (track == mSelectedTrack)
		{
			continue;
		}

		if (synth.getAutomationTrack(track).numNodes > 0)
		{
			SDL_Rect nodeArea = getNodeArea(area, synth.getAutomationTrack(track).nodes[0]);
			int px = nodeArea.x + nodeArea.w / 2;
			int py = nodeArea.y + nodeArea.h / 2;

			for (int i = 1 ; i < synth.getAutomationTrack(track).numNodes ; ++i)
			{
				SDL_Rect nodeArea = getNodeArea(area, synth.getAutomationTrack(track).nodes[i]);
				int x = nodeArea.x + nodeArea.w / 2;
				int y = nodeArea.y + nodeArea.h / 2;

				renderer.renderLine(px, py, x, y, track == mSelectedTrack ? Color(255, 255, 255) : Color(64, 64, 64));

				px = x;
				py = y;
			}
		}
	}


	if (getAutomationTrack().numNodes > 0)
	{
		SDL_Rect nodeArea = getNodeArea(area, 0);
		int px = nodeArea.x + nodeArea.w / 2;
		int py = nodeArea.y + nodeArea.h / 2;

		for (int i = 1 ; i < getAutomationTrack().numNodes ; ++i)
		{
			SDL_Rect nodeArea = getNodeArea(area, i);
			int x = nodeArea.x + nodeArea.w / 2;
			int y = nodeArea.y + nodeArea.h / 2;

			renderer.renderLine(px, py, x, y, Color(255, 255, 255));

			px = x;
			py = y;
		}

		for (int i = 0 ; i < getAutomationTrack().numNodes ; ++i)
		{
			SDL_Rect nodeArea = getNodeArea(area, i);
			renderer.clearRect(nodeArea, mSelectedNode == i ? Color(255, 255, 255) : Color(255, 0, 0));
		}
	}

	const SDL_Rect textArea = { area.x + area.w - renderer.getFontWidth() * 2 - 1, area.y + 1, 16, 16 };
	renderer.renderTextV(textArea, Color(192, 140, 32), "%02d", mSelectedTrack);
}


bool AutomationEditor::pickNode(int x, int y, const SDL_Rect& area, int& nodeOut) const
{
	SDL_Point point = {x,y};

	for (int i = 0 ; i < getAutomationTrack().numNodes ; ++i)
	{
		if (pointInRect(point, getNodeArea(area, i)))
		{
			nodeOut = i;
			return true;
		}
	}

	return false;
}


bool AutomationEditor::onEvent(SDL_Event& event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// Editor base class should probably do this when clicked
		setFocus(this);

		mMouseX = event.button.x / SCALE;
		mMouseY = event.button.y / SCALE;

		switch (event.button.button)
		{
			case SDL_BUTTON_LEFT:
				if (event.button.clicks == 1)
				{
					int nodeIdx;
					if (pickNode(mMouseX, mMouseY, mThisArea, nodeIdx))
					{
						Synth::AutomationNode& node = getAutomationTrack().getNode(nodeIdx);
						mSelectedNode = nodeIdx;
					}
					else
					{
						mSelectedNode = -1;
					}
					setDirty(true);
				}
				else if (event.button.clicks == 2)
				{
					Synth& synth = static_cast<Synth&>(mSynth);
					SDL_Keymod modState = SDL_GetModState();
					int xGranularity = 1, yGranularity = 1;

					if (modState & KMOD_CTRL)
					{
						xGranularity = synth.getPatternLength();
						yGranularity = mThisArea.h / 2;
					}

					mSelectedNode = getAutomationTrack().addNode(
						granularize(mMouseX - mThisArea.x + getScrollPosition(), xGranularity),
						1.0f - static_cast<float>(granularize(mMouseY - mThisArea.y, yGranularity)) / mThisArea.h
					);
					setDirty(true);
				}
				break;
		}

		return false;
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		mMouseX = event.motion.x / SCALE;
		mMouseY = event.motion.y / SCALE;

		if ((event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) && mSelectedNode != -1)
		{
			Synth& synth = static_cast<Synth&>(mSynth);
			SDL_Keymod modState = SDL_GetModState();
			int xGranularity = 1, yGranularity = 1;
			Synth::AutomationTrack& track = getAutomationTrack();
			Synth::AutomationNode& firstNode = track.getNode(mSelectedNode);
			int origTime = firstNode.time;
			float origValue = firstNode.value;

			if (modState & KMOD_CTRL)
			{
				xGranularity = synth.getPatternLength() / 2;
				yGranularity = mThisArea.h / 2;
			}

			track.editNode(mSelectedNode,
				granularize(mMouseX - mThisArea.x + getScrollPosition(), xGranularity),
				1.0f - static_cast<float>(granularize(mMouseY - mThisArea.y, yGranularity)) / mThisArea.h
			);

			if (modState & KMOD_ALT)
			{

				int deltaTime = firstNode.time - origTime;
				float deltaValue = firstNode.value - origValue;

				for (int nodeIdx = mSelectedNode + 1 ; nodeIdx < track.numNodes ; ++nodeIdx)
				{
					Synth::AutomationNode node = track.getNode(nodeIdx);
					track.editNode(nodeIdx,
						node.time + deltaTime,
						node.value + deltaValue
					);
				}
			}
		}
	}
	else if (event.type == SDL_MOUSEWHEEL)
	{
		mSelectedTrack = (mSelectedTrack + (event.wheel.y < 0 ? -1 : 1) + Synth::maxAutomationTracks) % Synth::maxAutomationTracks;
		mSelectedNode = -1;
		setDirty(true);
	}
	else if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
			case SDLK_DELETE:
				if (mSelectedNode != -1)
				{
					getAutomationTrack().removeNode(mSelectedNode);
					setDirty(true);
					mSelectedNode = -1;
				}
				break;
		}
	}

	return false;
}


void AutomationEditor::onListenableChange(Listenable *listenable)
{

}


void AutomationEditor::onLoaded()
{

}


int AutomationEditor::granularize(int value, int steps)
{
	int newValue = value + steps / 2;
	newValue = newValue - (newValue % steps);
	return abs(newValue - value) < 4 ? newValue : value;
}


Synth& AutomationEditor::getSynth()
{
	return static_cast<Synth&>(mSynth);
}


Synth::AutomationTrack& AutomationEditor::getAutomationTrack()
{
	return getSynth().getAutomationTrack(mSelectedTrack);
}


const Synth& AutomationEditor::getSynth() const
{
	return static_cast<const Synth&>(mSynth);
}


const Synth::AutomationTrack& AutomationEditor::getAutomationTrack() const
{
	return getSynth().getAutomationTrack(mSelectedTrack);
}


int AutomationEditor::getScrollPosition() const
{
	return std::max(0, mEditorState.patternEditor.currentRow + mEditorState.sequenceEditor.currentRow * getSynth().getPatternLength() - mThisArea.w / 2);
}
