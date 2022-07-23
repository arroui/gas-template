// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameData.h"
#include "<PROJECT_NAME>AssetManager.h"

U<PROJECT_NAME>GameData::U<PROJECT_NAME>GameData()
{
}

const U<PROJECT_NAME>GameData& U<PROJECT_NAME>GameData::U<PROJECT_NAME>GameData::Get()
{
	return U<PROJECT_NAME>AssetManager::Get().GetGameData();
}