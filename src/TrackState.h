#pragma once

#include "ITrackState.h"

struct TrackState: public ITrackState
{
	int effectValues[256];
	
	TrackState();
	
	virtual bool handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState);
	virtual void handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState);
};

