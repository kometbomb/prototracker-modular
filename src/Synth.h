#pragma once

#include "ISynth.h"
#include "SectionListener.h"

struct IPlayer;
struct Song;

class Synth: public ISynth, public SectionListener
{
public:
	static const int maxAutomationNodes = 1024;
	static const int maxAutomationTracks = 8;

	struct AutomationNode {
		int time;
		float value;
	};

	struct AutomationTrack {
		AutomationNode nodes[maxAutomationNodes];
		int numNodes;

		const AutomationNode& getNode(int index) const;
		AutomationNode& getNode(int index);
		int addNode(int time, float value);
		void removeNode(int index);
		void editNode(int index, int time, float value);
	};

private:
	AutomationTrack mAutomationTrack[maxAutomationTracks];

	void initAutomation();
	IPlayer& mPlayer;
	const Song& mSong;

public:

	Synth(IPlayer& player, const Song& song);
	virtual ~Synth();

	float getAutomationValue(int channel) const;

	AutomationTrack& getAutomationTrack(int track);
	const AutomationTrack& getAutomationTrack(int track) const;
	float getSongPosition() const;
	int getPatternLength() const;

	virtual void reset();
	
	virtual bool onFileSectionLoad(const FileSection& section, int& offset);
	virtual void onFileSectionSave(FileSection& section);
};
