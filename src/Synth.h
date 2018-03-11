#pragma once

#include "ISynth.h"
#include "SectionListener.h"

struct IPlayer;

class Synth: public ISynth, public SectionListener
{
public:
	Synth(IPlayer& player);
	virtual ~Synth();

	virtual void reset();

	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);
};
