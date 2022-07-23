// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_<PROJECT_NAME>ContextEffectsLibrary.h"
#include "Feedback/ContextEffects/<PROJECT_NAME>ContextEffectsLibrary.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_<PROJECT_NAME>ContextEffectsLibrary::GetSupportedClass() const
{
	return U<PROJECT_NAME>ContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
