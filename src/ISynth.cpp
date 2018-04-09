#include "Debug.h"
#include "ISynth.h"
#include "IOscillator.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "SDL.h"
#include <algorithm>

const int ISynth::oscillatorProbeLength;

ISynth::ISynth()
	: mProbePosition(0)
{
	mOscillator = new IOscillator*[SequenceRow::maxTracks];
	mRenderBuffers = new Sample16*[SequenceRow::maxTracks];
	mPreviousOscillatorOutput = new Sample16[oscillatorProbeLength * SequenceRow::maxTracks];

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mRenderBuffers[i] = new Sample16[2048];
	}

	mNumThreads = std::min(SDL_GetCPUCount(), SequenceRow::maxTracks);

	debug("Starting %d threads (max %d threads)", mNumThreads, SDL_GetCPUCount());
	mThreads = new Thread[mNumThreads];

	SDL_memset(mPreviousOscillatorOutput, 0, sizeof(Sample16) * oscillatorProbeLength * SequenceRow::maxTracks);
}


ISynth::~ISynth()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		delete mOscillator[i];

	delete[] mOscillator;

	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		delete mRenderBuffers[i];

	delete[] mRenderBuffers;
	delete[] mPreviousOscillatorOutput;
	delete[] mThreads;
}


ISynth::Thread::Thread()
{
	SDL_AtomicSet(&done, 0);
	inSem = SDL_CreateSemaphore(0);
	outSem = SDL_CreateSemaphore(0);
	start();
}


ISynth::Thread::~Thread()
{
	stop();
	SDL_DestroySemaphore(inSem);
	SDL_DestroySemaphore(outSem);
}


void ISynth::Thread::start()
{
	debug("[%p] Starting thread", this);

	thread = SDL_CreateThread(threadProc, "SynthThread", this);
}


void ISynth::Thread::stop()
{
	debug("[%p] Stopping thread", this);

	SDL_AtomicSet(&done, 1);
	SDL_SemPost(inSem);
	SDL_WaitThread(thread, NULL);
}


void ISynth::Thread::wait()
{
	SDL_SemWait(outSem);
}


void ISynth::Thread::render(IOscillator *_oscillator, Sample16 *_buffer, int _numSamples)
{
	oscillator = _oscillator;
	numSamples = _numSamples;
	buffer = _buffer;
	SDL_SemPost(inSem);
}


int ISynth::Thread::threadProc(void *data)
{
	Thread *thread = static_cast<Thread*>(data);

	while (true)
	{
		SDL_SemWait(thread->inSem);

		if (SDL_AtomicGet(&thread->done))
		{
			break;
		}

		SDL_memset(thread->buffer, 0, sizeof(Sample16) * thread->numSamples);
		thread->oscillator->render(thread->buffer, thread->numSamples);
		SDL_SemPost(thread->outSem);
	}

	return 0;
}


IOscillator& ISynth::getOscillator(int i)
{
	return *mOscillator[i];
}


const Sample16* ISynth::getOscillatorProbe(int oscillator) const
{
	return &mPreviousOscillatorOutput[oscillatorProbeLength * oscillator];
}


void ISynth::update(int numSamples)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i]->update(numSamples);
}


void ISynth::render(Sample16 *buffer, int numSamples)
{
	SDL_memset(buffer, 0, sizeof(Sample16) * numSamples);

	int probeCount = std::min(numSamples, oscillatorProbeLength);

	for (int channel = 0 ; channel < SequenceRow::maxTracks ; )
	{
		// Run each thread/channel
		for (int i = 0 ; i < mNumThreads && channel < SequenceRow::maxTracks ; ++i)
		{
			mThreads[i].render(mOscillator[channel], mRenderBuffers[channel], numSamples);
			channel++;
		}

		// Wait for all threads
		for (int i = 0 ; i < mNumThreads ; ++i)
		{
			mThreads[i].wait();
		}
	}

	// Mix results from all threads
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		for (int p = 0 ; p < numSamples ; ++p)
		{
			buffer[p].left += mRenderBuffers[i][p].left;
			buffer[p].right += mRenderBuffers[i][p].right;
		}

		Sample16 *src = mRenderBuffers[i] + std::max(0, numSamples - oscillatorProbeLength);
		Sample16 *dest = mPreviousOscillatorOutput + oscillatorProbeLength * i;

		for (int p = 0 ; p < probeCount ; ++p)
		{
			Sample16& sample = dest[(p + mProbePosition) % oscillatorProbeLength];
			sample.left = src->left;
			sample.right = src->right;
			src++;
		}
	}

	mProbePosition += probeCount;
}


int ISynth::getProbePosition() const
{
	return mProbePosition;
}


void ISynth::setSampleRate(int rate)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i]->setSampleRate(rate);
}


void ISynth::reset()
{
}
