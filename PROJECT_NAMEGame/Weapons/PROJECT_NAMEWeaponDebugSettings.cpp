// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>WeaponDebugSettings.h"
#include "Misc/App.h"

U<PROJECT_NAME>WeaponDebugSettings::U<PROJECT_NAME>WeaponDebugSettings()
{
}

FName U<PROJECT_NAME>WeaponDebugSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
