#pragma once

#include <vector>

#include "SynthModule.h"

struct ModularSynth;

class ModuleFactory
{
public:
	struct Module {
		int id, maxInstances;
		const char *name;
		SynthModule::CreateModuleFunc createFunc;
		
		Module(int _id, const char *_name, SynthModule::CreateModuleFunc _create, int _maxInstances): id(_id), maxInstances(_maxInstances), name(_name), createFunc(_create) {}
	};
private:
	std::vector<Module> mModules;
	void registerModule(int id, const char *name, SynthModule::CreateModuleFunc createFunc, int maxInstances);
	
public:
	ModuleFactory();

	SynthModule* createModule(int id, ModularSynth& synth) const;
	int getNumModules() const;
	const Module& getModuleInfo(int index) const;
};
