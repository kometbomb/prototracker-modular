#pragma once

#include <vector>

#include "SynthModule.h"

struct ModularSynth;

class ModuleFactory
{
public:
	struct Module {
		const int id, maxInstances;
		const char *name;
		const SynthModule::ModuleClass moduleClass;
		SynthModule::CreateModuleFunc createFunc;

		Module(SynthModule::ModuleClass _moduleClass, int _id, const char *_name, SynthModule::CreateModuleFunc _create, int _maxInstances)
			: id(_id), moduleClass(_moduleClass), maxInstances(_maxInstances), name(_name), createFunc(_create) {}
	};
private:
	std::vector<Module> mModules;
	void registerModule(SynthModule::ModuleClass moduleClass, int id, const char *name, SynthModule::CreateModuleFunc createFunc, int maxInstances);

public:
	ModuleFactory();

	SynthModule* createModule(int id, ModularSynth& synth) const;
	int getNumModules() const;
	const Module& getModuleInfo(int index) const;
};
