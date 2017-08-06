#include "ModuleFactory.h"
#include "OscillatorModule.h"
#include "PassthruModule.h"
#include <cstdlib>

#define REGISTER(module) registerModule(module::moduleId, module::moduleName, &module::createModule)

ModuleFactory::ModuleFactory()
{
	REGISTER(OscillatorModule);
	REGISTER(PassthruModule);
}


void ModuleFactory::registerModule(int id, const char *name, SynthModule::CreateModuleFunc createModule)
{
	mModules.push_back(Module(id, name, createModule));
}


SynthModule* ModuleFactory::createModule(int id) const
{
	for (int i = 0 ; i < mModules.size() ; ++i)
		if (mModules[i].id == id)
			return mModules[i].createFunc();
			
	return NULL;
}


int ModuleFactory::getNumModules() const
{
	return mModules.size();
}


const ModuleFactory::Module& ModuleFactory::getModuleInfo(int index) const
{
	return mModules[index];
}
