#pragma once

#include "ISynth.h"
#include "SectionListener.h"

class Synth: public ISynth, public SectionListener
{
public:
	Synth();
	virtual ~Synth();
	
	virtual void reset();
	
	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);
};
