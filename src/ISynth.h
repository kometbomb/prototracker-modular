#pragma once

struct IOscillator;
struct Sample16;

/*

ISynth handles the IOscillators and some things needed by the GUI
(namely, data for visualization).

Override ISynth() to initialize your own IOscillators.

*/

#include "SDL.h"

class ISynth
{
	struct Thread
	{
		SDL_atomic_t done;
		IOscillator *oscillator;
		Sample16* buffer;
		int numSamples;
		SDL_Thread *thread;
		SDL_semaphore *inSem, *outSem;

		Thread();
		~Thread();

		void render(IOscillator *oscillator, int numSamples);

		void start();
		void stop();
		void wait();

		static int threadProc(void *data);
	};

protected:
	IOscillator **mOscillator;
	Sample16 *mPreviousOscillatorOutput;
	Thread *mThreads;
	int mProbePosition;

public:
	ISynth();
	virtual ~ISynth();

	virtual void reset();

	static const int oscillatorProbeLength = 128;

	const Sample16* getOscillatorProbe(int oscillator) const;
	IOscillator& getOscillator(int i);
	int getProbePosition() const;
	void setSampleRate(int rate);

	// Update the synth state
	virtual void update(int numSamples);

	/*
	 Get samples (does not change synth state)
	 Note: ISynth should overwrite the values in the buffer;
	 it might contain random values. Fill with zeroes if
	 the output should be silent.

	 This should also update mPreviousOscillatorOutput for
	 the GUI.
	*/
	virtual void render(Sample16 *buffer, int numSamples);
};
