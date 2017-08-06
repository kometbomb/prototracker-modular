#include "Synth.h"
#include "ModularSynth.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "SDL.h"

Synth::Synth()
	: ISynth()
{
	/* 
	
	Initialize the audio tracks.
	
	*/
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mOscillator[i] = new ModularSynth();
	}
}


Synth::~Synth()
{
	/*
	
	NOTE: ~ISynth() will delete the Oscillator objects we initialized
	above! No need to cleanup yourself.
	
	*/
}
