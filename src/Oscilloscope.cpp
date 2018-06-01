#include "Oscilloscope.h"
#include "Renderer.h"
#include "Color.h"
#include "Sample.h"
#include "Value.h"
#include "IPlayer.h"
#include "ITrackState.h"
#include "IOscillator.h"

Oscilloscope::Oscilloscope(EditorState& editorState, IPlayer& player, int channel)
	: Editor(editorState, false), mPlayer(player), mChannel(channel), mOscilloscopePos(0)
{
}

	
Oscilloscope::~Oscilloscope()
{
}


void Oscilloscope::setBuffer(const Sample16 *buffer, int length)
{
	mBuffer = buffer;
	mBufferLength = length;
	setDirty(true);
}


void Oscilloscope::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);
	renderer.renderBackground(area);
	
	SDL_Point points[area.w];
	SDL_Rect block = {};
	SDL_Rect textPos = {};
	
	if (mPlayer.getTrackState(mChannel).enabled)
	{
		for (int x = 0 ; x < area.w ; ++x)
		{
			int bufferPos = mBufferLength * x / area.w;
			const Sample16& sample = mBuffer[(bufferPos + mOscilloscopePos) % mBufferLength];
			int y = -(sample.left + sample.right) * area.h / IOscillator::oscillatorResolution / 2 + area.h / 2;
			
			if (y >= area.h)
				y = area.h - 1;
			else if (y < 0)
				y = 0;
			
			points[x].x = area.x + x;
			points[x].y = area.y + y;
		}
		
		renderer.renderPoints(points, area.w, Color());
	}
	else
	{
		for (int x = 0 ; x < area.w ; ++x)
		{
			points[x].x = area.x + x;
			points[x].y = area.h / 2 + 2;
		}
		
		block = {area.x + 10, area.h / 2 - 2, 35, 9};
		textPos = {block.x + 2, block.y + 1, 33, 8};
		
		renderer.renderPoints(points, area.w, Color());
		renderer.renderRect(block, Color(255,0,0));
		renderer.renderText(textPos, Color(), "mute");
	}
}


bool Oscilloscope::onEvent(SDL_Event& event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		mPlayer.getTrackState(mChannel).enabled ^= true;
		return true;
	}
	
	return false;
}


void Oscilloscope::onListenableChange(Listenable *listenable)
{
	setDirty(true);
	
	// Assumes this will always be called by mOscillatorsProbePos->notify()
	mOscilloscopePos = *static_cast<Value*>(listenable);
}
