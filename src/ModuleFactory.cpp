#include "ModuleFactory.h"
#include "modules/OscillatorModule.h"
#include "modules/NoiseModule.h"
#include "modules/PulseModule.h"
#include "modules/PassthruModule.h"
#include "modules/AddModule.h"
#include "modules/MulModule.h"
#include "modules/AbsModule.h"
#include "modules/AutomationModule.h"
#include "modules/ConstModule.h"
#include "modules/EffectModule.h"
#include "modules/EGModule.h"
#include "modules/RMSModule.h"
#include "modules/VolumeModule.h"
#include "modules/FilterModule.h"
#include "modules/GlideModule.h"
#include "modules/SplitModule.h"
#include "modules/MixerModule.h"
#include "modules/ClampModule.h"
#include "modules/DelayModule.h"
#include "modules/ReverbModule.h"
#include "modules/ShapeModule.h"
#include "modules/BitsModule.h"
#include "modules/DistortionModule.h"
#include "modules/AccumulatorModule.h"
#include "modules/AudioOutModule.h"
#include "modules/FrequencyInModule.h"
#include "modules/TransposeModule.h"
#include "modules/TriggerNoteModule.h"
#include "modules/TickModule.h"
#include "modules/LinearModule.h"
#include "modules/LerpModule.h"
#include "modules/OscilloscopeModule.h"
#include "modules/VUMeterModule.h"
#include "modules/ContainerModule.h"
#include "modules/VirtualModule.h"
#include "modules/ExtInModule.h"
#include "modules/ExtOutModule.h"
#include <cstdlib>

#define REGISTER(module) registerModule(module::moduleId, module::moduleName, &module::createModule, module::maxInstances)

ModuleFactory::ModuleFactory()
{
	REGISTER(OscillatorModule);
	REGISTER(NoiseModule);
	REGISTER(PulseModule);
	REGISTER(EGModule);
	REGISTER(PassthruModule);
	REGISTER(AddModule);
	REGISTER(MulModule);
	REGISTER(AbsModule);
	REGISTER(RMSModule);
	REGISTER(TickModule);
	REGISTER(ConstModule);
	REGISTER(EffectModule);
	REGISTER(AutomationModule);
	REGISTER(SplitModule);
	REGISTER(MixerModule);
	REGISTER(ClampModule);
	REGISTER(ShapeModule);
	REGISTER(LinearModule);
	REGISTER(LerpModule);
	REGISTER(DistortionModule);
	REGISTER(BitsModule);
	REGISTER(DelayModule);
	REGISTER(ReverbModule);
	REGISTER(FilterModule);
	REGISTER(GlideModule);
	REGISTER(AccumulatorModule);
	REGISTER(FrequencyInModule);
	REGISTER(VolumeModule);
	REGISTER(TransposeModule);
	REGISTER(TriggerNoteModule);
	REGISTER(OscilloscopeModule);
	REGISTER(VUMeterModule);
	REGISTER(AudioOutModule);
	REGISTER(ContainerModule);
	REGISTER(VirtualModule);
	REGISTER(ExtInModule);
	REGISTER(ExtOutModule);
}


void ModuleFactory::registerModule(int id, const char *name, SynthModule::CreateModuleFunc createModule, int maxInstances)
{
	mModules.push_back(Module(id, name, createModule, maxInstances));
}


SynthModule* ModuleFactory::createModule(int id, ModularSynth& synth) const
{
	for (int i = 0 ; i < mModules.size() ; ++i)
		if (mModules[i].id == id)
			return mModules[i].createFunc(synth);

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
