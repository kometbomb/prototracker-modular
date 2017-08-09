#pragma once

#include "SynthConnection.h"
#include "IOscillator.h"

struct SynthModule;
struct FileSection;

class ModularSynth: public IOscillator
{
public:
	static const int maxModules = 16;
	static const int maxConnections = 64;
	static const int outputResolution = 1000;

private:
	SynthModule *mModules[maxModules];
	SynthConnection mConnections[maxConnections];
	int mNumConnections;
	float mFrequency, mVolume, mOutput[2];
	bool mNoteTrigger;
	
	void cycle();
	
public:
	ModularSynth();
	virtual ~ModularSynth();
	
	const SynthModule* getModule(int index) const;
	const SynthConnection& getConnection(int index) const;
	int getNumConnections() const;
	
	bool addModule(int index, int moduleId);
	void removeModule(int index);
	bool connectModules(int fromModule, int toModule, int fromOutput, int toInput);
	void detachConnection(int moduleIndex, int type, int connectionIndex);
	void removeConnection(int index);
	void swapModules(int fromModule, int toModule);
	
	void clear();
	
	// Disk ops
	void writeSynth(FileSection& section);
	bool readSynth(const FileSection& section, int& offset);
	
	// For communication with SynthModules
	float getFrequency() const;
	bool getNoteTrigger() const;
	void setMasterOutput(int channel, float output);

	// IOscillator virtual methods
	virtual void triggerNote();
	virtual void setFrequency(float frequency);
	virtual void setVolume(int volume);
	virtual void update(int numSamples);
	virtual void render(Sample16 *buffer, int numSamples, int offset = 0);
};
