#include "TrackState.h"
#include "EffectParam.h"
#include "SDL.h"

TrackState::TrackState()
	: ITrackState()
{
	SDL_memset(effectValues, 0, sizeof(effectValues));
}


bool TrackState::handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState)
{
	int asByte = effect.getParamsAsByte();
	
	effectValues[effect.effect] = asByte;
	
	switch (effect.effect)
	{
		/*
		
		In case we didn't handle the effect, let the parent class handle it.
		
		*/
		
		default:
			return ITrackState::handleEffectZeroTick(effect, playerState);
	}
	
	return false;
}


void TrackState::handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState)
{
	ITrackState::handleEffectAnyTick(effect, playerState);
}
