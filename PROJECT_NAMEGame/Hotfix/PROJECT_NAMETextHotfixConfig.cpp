// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TextHotfixConfig.h"
#include "Internationalization/TextLocalizationManager.h"

U<PROJECT_NAME>TextHotfixConfig::U<PROJECT_NAME>TextHotfixConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>TextHotfixConfig::ApplyTextReplacements() const
{
	FTextLocalizationManager::Get().RegisterPolyglotTextData(TextReplacements);
}

void U<PROJECT_NAME>TextHotfixConfig::PostInitProperties()
{
	Super::PostInitProperties();
	ApplyTextReplacements();
}

void U<PROJECT_NAME>TextHotfixConfig::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	ApplyTextReplacements();
}

#if WITH_EDITOR
void U<PROJECT_NAME>TextHotfixConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplyTextReplacements();
}
#endif
