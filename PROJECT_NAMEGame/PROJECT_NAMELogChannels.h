// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

<PROJECT_NAME_UPPER>GAME_API DECLARE_LOG_CATEGORY_EXTERN(Log<PROJECT_NAME>, Log, All);
<PROJECT_NAME_UPPER>GAME_API DECLARE_LOG_CATEGORY_EXTERN(Log<PROJECT_NAME>Experience, Log, All);
<PROJECT_NAME_UPPER>GAME_API DECLARE_LOG_CATEGORY_EXTERN(Log<PROJECT_NAME>AbilitySystem, Log, All);
<PROJECT_NAME_UPPER>GAME_API DECLARE_LOG_CATEGORY_EXTERN(Log<PROJECT_NAME>Teams, Log, All);

<PROJECT_NAME_UPPER>GAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
