#include "OscilloscopeModule.h"
#include "../Renderer.h"
#include "../Color.h"
#include <cstdio>
#include <algorithm>

OscilloscopeModule::OscilloscopeModule(ModularSynth& synth)
	:SynthModule(synth, moduleId, 3, 1, 0), mHead(0), mBuffer(NULL),
	mBufferLength(0), mPrevInput(0), mSampleStart(0), mSampleEnd(0),
	mSampleLength(0), mLastUpdated(0)
{
	initBuffer();
}


OscilloscopeModule::~OscilloscopeModule()
{
	delete [] mBuffer;
}


void OscilloscopeModule::initBuffer()
{
	if (mBuffer != NULL)
	{
		delete[] mBuffer;
		mBuffer = NULL;
	}

	if (mSampleRate > 0)
	{
		mBufferLength = mSampleRate * bufferLengthMs / 1000;
		mBuffer = new float[mBufferLength];
		SDL_memset(mBuffer, 0, sizeof(mBuffer[0]) * mBufferLength);
	}

	mHead = 0;
	mSampleLength = 0;
}


void OscilloscopeModule::cycle()
{
	float input = getInput(0);
	float syncedInput = getInput(1);
	float sync = getInput(2);
	float combinedInput = input + syncedInput;

	mBuffer[mHead] = combinedInput;

	// Update on zero crossing (between this and previous cycle on input #1)
	// or on raising sync input or if more than half of the buffer has been
	// used since last update.

	if ((mPrevInput <= 0 && syncedInput > 0) || (mPrevSync <= 0.5f && sync > 0.5f)
		|| mLastUpdated >= mBufferLength / 2)
	{
		mSampleStart = mSampleEnd;
		mSampleEnd = mHead;
		mSampleLength = (mSampleEnd - mSampleStart + mBufferLength) % mBufferLength;
		mLastUpdated = 0;
	}

	if (++mHead >= mBufferLength)
	{
		mHead = 0;
	}

	mPrevInput = syncedInput;
	mPrevSync = sync;
	++mLastUpdated;

	setOutput(0, combinedInput);
}



const char * OscilloscopeModule::getInputName(int input) const
{
	static const char *names[] = {"Input", "Auto", "Sync"};
	return names[input];
}


const char * OscilloscopeModule::getOutputName(int output) const
{
	static const char *names[] = {"Passthru"};
	return names[output];
}


const char * OscilloscopeModule::getName() const
{
	return "Oscilloscope";
}


SynthModule * OscilloscopeModule::createModule(ModularSynth& synth)
{
	return new OscilloscopeModule(synth);
}


void OscilloscopeModule::setSampleRate(int newRate)
{
	SynthModule::setSampleRate(newRate);
	initBuffer();
}


void OscilloscopeModule::render(Renderer& renderer, const SDL_Rect& moduleArea, bool isSelected) const
{
	renderer.renderRect(moduleArea, getModuleColor(isSelected));
	renderer.renderLine(moduleArea.x, moduleArea.y + moduleArea.h / 2, moduleArea.x + moduleArea.w - 1, moduleArea.y + moduleArea.h / 2, Color(160, 160, 160));

	if (mBufferLength == 0 || mSampleLength == 0)
		return;

	int bufPos = mSampleStart;
	int maxAmp = moduleArea.h / 2;
	int py = std::max(-maxAmp, std::min(maxAmp - 1, static_cast<int>(mBuffer[bufPos] * maxAmp)));
	switch (mType)
	{
		default:
		case 0: {
			// normal
			for (int x = 1 ; x < moduleArea.w ; ++x)
			{
				bufPos = (x * mSampleLength / moduleArea.w + mSampleStart) % mBufferLength;
				int y = std::max(-maxAmp, std::min(maxAmp - 1, static_cast<int>(mBuffer[bufPos] * maxAmp)));
				renderer.renderLine(moduleArea.x + x - 1, -py + moduleArea.y + moduleArea.h / 2, moduleArea.x + x, -y + moduleArea.y + moduleArea.h / 2, Color(255, 255, 255));
				
				//	y values were drawn inverted onto the rect due to point of origin being 0 (top of rectangle)
				//	and they were being added to the height offset (so negative values showed above zero
				//	line & positive below zero) inverting the calculated y variables seems to fix this
				
				py = y;
			}
		} break;
			
		case 1: {
			// inverted
			for (int x = 1 ; x < moduleArea.w ; ++x)
			{
				bufPos = (x * mSampleLength / moduleArea.w + mSampleStart) % mBufferLength;
				int y = std::max(-maxAmp, std::min(maxAmp - 1, static_cast<int>(mBuffer[bufPos] * maxAmp)));
				renderer.renderLine(moduleArea.x + x - 1, py + moduleArea.y + moduleArea.h / 2, moduleArea.x + x, y + moduleArea.y + moduleArea.h / 2, Color(255, 255, 255));
				
				py = y;
			}

		} break;
			
	}

}

void OscilloscopeModule::onDial(int delta)
{
	mType = (static_cast<int>(static_cast<float>(delta) + mParams[0]) + 2) % 2;
	setParam(0, mType);
}
