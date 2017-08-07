#include "Synth.h"
#include "ModularSynth.h"
#include "SequenceRow.h"
#include "FileSection.h"
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


bool Synth::onFileSectionLoad(const FileSection& section, int& offset)
{
	int synthCount = section.readByte(offset);
	
	if (synthCount == FileSection::invalidRead || synthCount > SequenceRow::maxTracks)
		return false;
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		static_cast<ModularSynth*>(mOscillator[i])->readSynth(section, offset);
	}
	
	return true;
}


void Synth::onFileSectionSave(FileSection& section)
{
	section.writeByte(SequenceRow::maxTracks);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		static_cast<ModularSynth*>(mOscillator[i])->writeSynth(section);
	}
}
