// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>RuntimeOptions.h"

U<PROJECT_NAME>RuntimeOptions::U<PROJECT_NAME>RuntimeOptions()
{
	OptionCommandPrefix = TEXT("ro");
}

U<PROJECT_NAME>RuntimeOptions* U<PROJECT_NAME>RuntimeOptions::GetRuntimeOptions()
{
	return GetMutableDefault<U<PROJECT_NAME>RuntimeOptions>();
}

const U<PROJECT_NAME>RuntimeOptions& U<PROJECT_NAME>RuntimeOptions::Get()
{
	const U<PROJECT_NAME>RuntimeOptions& RuntimeOptions = *GetDefault<U<PROJECT_NAME>RuntimeOptions>();
	return RuntimeOptions;
}