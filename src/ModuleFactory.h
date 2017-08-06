#pragma once

#include <vector>

#include "SynthModule.h"

class ModuleFactory
{
public:
	struct Module {
		int id;
		const char *name;
		SynthModule::CreateModuleFunc createFunc;
		
		Module(int _id, const char *_name, SynthModule::CreateModuleFunc _create): id(_id), name(_name), createFunc(_create) {}
	};
private:
	std::vector<Module> mModules;
	void registerModule(int id, const char *name, SynthModule::CreateModuleFunc createFunc);
	
public:
	ModuleFactory();

	SynthModule* createModule(int id) const;
	int getNumModules() const;
	const Module& getModuleInfo(int index) const;
};
