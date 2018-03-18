#include "Synth.h"
#include "ModularSynth.h"
#include "SequenceRow.h"
#include "FileSection.h"
#include "Sample.h"
#include "SDL.h"
#include <cstring>

Synth::Synth(IPlayer& player)
	: ISynth()
{
	/*

	Initialize the audio tracks.

	*/

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mOscillator[i] = new ModularSynth(player);
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
	if (strcmp(section.getName(), "SYNT") == 0)
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
	else
	{
		return false;
	}
}


void Synth::onFileSectionSave(FileSection& section)
{
	if (strcmp(section.getName(), "SYNT") == 0)
	{
		section.writeByte(SequenceRow::maxTracks);

		for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		{
			static_cast<ModularSynth*>(mOscillator[i])->writeSynth(section);
		}
	}
}


void Synth::onSongClear()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		static_cast<ModularSynth*>(mOscillator[i])->clear();
}


void Synth::reset()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		static_cast<ModularSynth*>(mOscillator[i])->clear();
}
