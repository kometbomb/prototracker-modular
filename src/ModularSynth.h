#pragma once

#include "SynthConnection.h"
#include "IOscillator.h"
#include "Lockable.h"

struct SynthModule;
struct FileSection;
struct Synth;
struct IPlayer;

class ModularSynth: public IOscillator, public Lockable
{
public:
	static const int maxModules = 32;
	static const int maxConnections = 128;
	static const int outputResolution = 4096;
	static const int numExtConnections = 8;
	static const int nameLength = 16;

private:
	char mName[nameLength + 1];
	Synth& mSynth;
	IPlayer& mPlayer;
	SynthModule *mModules[maxModules];
	SynthConnection mConnections[maxConnections];
	int mNumConnections;
	float mFrequency, mVolume, mOutput[2];
	float mExtInput[numExtConnections], mExtOutput[numExtConnections];
	bool mNoteTrigger;
	int mEffectValues[256];

	// Samples since the synth last output non-silence
	int mSilenceLength;
	bool mIsPausable, mPaused;

	// Consider output under the threshold as silence
	static constexpr float silenceThreshold = 0.001f;

	// Synth will pause after this many samples of "silence"
	static const int silenceDurationUntilPause = 1000;

public:
	ModularSynth(Synth& synth, IPlayer& player, bool isPausable = false);
	virtual ~ModularSynth();

	const char *getName() const;
	char *getName();
	void setName(const char *name);

	void cycle();
	bool isPaused() const;

	ModularSynth* createEmpty() const;
	ModularSynth* clone() const;
	void copy(const ModularSynth& source);

	const SynthModule* getModule(int index) const;
	SynthModule* getModule(int index);
	const SynthConnection& getConnection(int index) const;
	int getNumConnections() const;

	void setExtInput(int index, float value);
	float getExtInput(int index) const;
	void setExtOutput(int index, float value);
	float getExtOutput (int index) const;

	bool addModule(int index, int moduleId);
	void removeModule(int index);
	bool connectModules(int fromModule, int toModule, int fromOutput, int toInput);
	void detachConnection(int moduleIndex, int type, int connectionIndex);
	void removeConnection(int index);
	void swapModules(int fromModule, int toModule);
	void onShow();

	void clear();

	// Disk ops
	void writeSynth(FileSection& section);
	bool readSynth(const FileSection& section, int& offset);

	// For communication with SynthModules
	float getFrequency() const;
	bool getNoteTrigger() const;
	void setMasterOutput(int channel, float output);
	int getEffectValue(int effect) const;
	float getAutomationValue(int track) const;
	int getSongRate() const;

	// IOscillator virtual methods
	virtual void triggerNote();
	virtual void setFrequency(float frequency);
	virtual void setVolume(int volume);
	virtual void update(int numSamples);
	virtual void render(Sample16 *buffer, int numSamples, int offset = 0);
	virtual void setSampleRate(int rate);
	virtual void handleTrackState(ITrackState& trackState);
};
