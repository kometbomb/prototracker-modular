#pragma once

#include "../SynthModule.h"

class ReverbModule: public SynthModule
{
	struct Filter {
		const int mLength;
		int mHead;
		float *mBuffer;

		Filter(int length);
		~Filter();
	};

	struct DelayFilter: public Filter {
		float mOutput;
		float cycle(float input);
		float getOutput() const;
		DelayFilter(int length);
	};

	struct AllpassFilter: public Filter {
		float cycle(float input, float gain);
		AllpassFilter(int length);
	};

	static const int numAllpassFiltersPerRow = 2;
	static const int numRows = 4;
	static const int numDelays = numRows;
	static const int numAllpassFilters = numAllpassFiltersPerRow * numRows;

	DelayFilter *mDelay[numDelays];
	AllpassFilter *mAllpass[numAllpassFilters];

	ReverbModule(ModularSynth& synth);

public:
	virtual ~ReverbModule();

	static const int moduleId = 25;
	static const int maxInstances = -1;
	static constexpr const char *moduleName = "Reverb";

	static SynthModule * createModule(ModularSynth& synth);

	virtual void cycle();

	virtual const char * getName() const;
	virtual const char * getInputName(int input) const;
	virtual const char * getOutputName(int output) const;

	virtual void setSampleRate(int newRate);
};
