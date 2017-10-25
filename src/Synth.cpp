#include "Synth.h"
#include "ModularSynth.h"
#include "SequenceRow.h"
#include "FileSection.h"
#include "Sample.h"
#include "Player.h"
#include "Song.h"
#include "SDL.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>

static const int nodeValueScale = 8192;
const int Synth::maxAutomationNodes;
const int Synth::maxAutomationTracks;

Synth::Synth(IPlayer& player, const Song& song)
	: ISynth(), mPlayer(player), mSong(song)
{
	/*

	Initialize automation tracks.

	*/

	initAutomation();

	/* 
	
	Initialize the audio tracks.
	
	*/
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mOscillator[i] = new ModularSynth(*this);
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
		// Synth layout

		int synthCount = section.readByte(offset);
		
		if (synthCount == FileSection::invalidRead || synthCount > SequenceRow::maxTracks)
			return false;
		
		for (int i = 0 ; i < synthCount ; ++i)
		{
			static_cast<ModularSynth*>(mOscillator[i])->readSynth(section, offset);
		}
		
		return true;
	}
	else if (strcmp(section.getName(), "AUTO") == 0)
	{
		// Automation

		initAutomation();

		int trackCount = section.readByte(offset);

		if (trackCount == FileSection::invalidRead || trackCount > maxAutomationTracks)
			return false;

		for (int i = 0 ; i < trackCount ; ++i)
		{
			int nodeCount = section.readDword(offset);

			if (nodeCount == FileSection::invalidRead || nodeCount > maxAutomationNodes)
			{
				return false;
			}

			mAutomationTrack[i].numNodes = nodeCount;

			for (int node = 0 ; node < nodeCount ; ++node)
			{
				int nodeTime = section.readDword(offset);
				int nodeValue = section.readDword(offset);

				if (nodeTime == FileSection::invalidRead || nodeValue == FileSection::invalidRead)
				{
					return false;
				}

				mAutomationTrack[i].nodes[node].time = std::max(0, nodeTime);
				mAutomationTrack[i].nodes[node].value = static_cast<float>(nodeValue) / nodeValueScale;
			}
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
	else if (strcmp(section.getName(), "AUTO") == 0)
	{
		section.writeByte(maxAutomationTracks);

		for (int i = 0 ; i < maxAutomationTracks ; ++i)
		{
			section.writeDword(mAutomationTrack[i].numNodes);

			for (int node = 0 ; node < mAutomationTrack[i].numNodes ; ++node)
			{
				section.writeDword(mAutomationTrack[i].nodes[node].time);
				section.writeDword(mAutomationTrack[i].nodes[node].value * nodeValueScale);
			}
		}
	}
}


void Synth::reset()
{
	initAutomation();

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		static_cast<ModularSynth*>(mOscillator[i])->clear();
}


void Synth::initAutomation() 
{
	for (int i = 0 ; i < maxAutomationTracks ; ++i) 
	{
		mAutomationTrack[i].numNodes = 0;
	}
}


float Synth::getAutomationValue(int channel) const
{
	float time = getSongPosition();
	int truncTime = trunc(time);

	const AutomationTrack& track = mAutomationTrack[channel];

	if (track.numNodes == 0)
	{
		return 0.0f;
	}

	if (time <= track.nodes[0].time) 
	{
		// Return the first value for OOB
		return track.nodes[0].value;
	}

	for (int i = 0 ; i < track.numNodes - 1 ; ++i)
	{
		int at = track.nodes[i].time;
		int bt = track.nodes[i + 1].time;

		if (at <= truncTime && bt > truncTime)
		{
			float a = track.nodes[i].value;
			float b = track.nodes[i + 1].value;

			if (at == bt)
			{
				return b;
			}

			float t = (time - at) / (bt - at);
			return (b - a) * t + a;
		}
	}

	// Return the last value for OOB
	return track.nodes[track.numNodes - 1].value;
}


const Synth::AutomationNode& Synth::AutomationTrack::getNode(int index) const
{
	return nodes[index];
}


Synth::AutomationNode& Synth::AutomationTrack::getNode(int index) 
{
	return nodes[index];
}


int Synth::AutomationTrack::addNode(int time, float value)
{
	if (numNodes >= maxAutomationNodes)
	{
		return -1;
	}

	int index = 0;
	while (index < numNodes)
	{
		if (nodes[index].time > time) 
		{
			break;
		}
		++index;
	}

	if (index >= maxAutomationNodes)
	{
		return -1;
	}

	if (numNodes > index)
	{
		memmove(&nodes[index + 1], &nodes[index], sizeof(nodes[0]) * (numNodes - index));
	}

	nodes[index].time = time;
	nodes[index].value = value;
	numNodes++;

	return index;
}


void Synth::AutomationTrack::removeNode(int index)
{
	if (index >= numNodes) 
	{
		return;
	}

	--numNodes;

	memmove(&nodes[index], &nodes[index + 1], sizeof(nodes[0]) * (numNodes - index));
}


const Synth::AutomationTrack& Synth::getAutomationTrack(int track) const
{
	return mAutomationTrack[track];
}


Synth::AutomationTrack& Synth::getAutomationTrack(int track)
{
	return mAutomationTrack[track];
}


void Synth::AutomationTrack::editNode(int index, int time, float value)
{
	value = std::min(1.0f, std::max(0.0f, value));
	time = std::max(0, time);

	if (index > 0)
	{
		time = std::max(time, nodes[index - 1].time);
	}

	if (index < numNodes - 1)
	{
		time = std::min(time, nodes[index + 1].time);
	}

	nodes[index].time = time;
	nodes[index].value = value;
}


float Synth::getSongPosition() const
{
	const PlayerState& state = mPlayer.getPlayerState();
	return state.patternRow + state.sequenceRow * mSong.getPatternLength() + static_cast<float>(state.tick) / std::max(1, state.songSpeed);
}


int Synth::getPatternLength() const
{
	return mSong.getPatternLength();
}
