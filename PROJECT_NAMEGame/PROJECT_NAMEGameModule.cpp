// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>LogChannels.h"
#include "Modules/ModuleManager.h"


/**
 * F<PROJECT_NAME>GameModule
 */
class F<PROJECT_NAME>GameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(F<PROJECT_NAME>GameModule, <PROJECT_NAME>Game, "<PROJECT_NAME>Game");
