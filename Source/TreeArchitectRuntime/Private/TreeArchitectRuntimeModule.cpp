//$ Copyright 2015-2016, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#include "TreeArchitectRuntimePrivatePCH.h"
#include "ITreeArchitectRuntimeModule.h"

class FTreeArchitectRuntime : public ITreeArchitectRuntime
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;
};

IMPLEMENT_MODULE( FTreeArchitectRuntime, TreeArchitectRuntime )

void FTreeArchitectRuntime::StartupModule()
{
	
}

void FTreeArchitectRuntime::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FTreeArchitectRuntime::SupportsDynamicReloading()
{
	return true;
}
