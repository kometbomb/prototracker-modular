#pragma once

#include "ISynth.h"
#include "SectionListener.h"

class Synth: public ISynth, public SectionListener
{
public:
	Synth();
	virtual ~Synth();
	
	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);
};
