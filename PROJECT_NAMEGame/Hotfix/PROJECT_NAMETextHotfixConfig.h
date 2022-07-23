// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Internationalization/PolyglotTextData.h"
#include "<PROJECT_NAME>TextHotfixConfig.generated.h"

/**
 * This class allows hotfixing individual FText values anywhere
 */

UCLASS(config=Game, defaultconfig)
class U<PROJECT_NAME>TextHotfixConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>TextHotfixConfig(const FObjectInitializer& ObjectInitializer);

	// UObject interface
	virtual void PostInitProperties() override;
	virtual void PostReloadConfig(FProperty* PropertyThatWasLoaded) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// End of UObject interface

private:
	void ApplyTextReplacements() const;

private:
	// The list of FText values to hotfix
	UPROPERTY(Config, EditAnywhere)
	TArray<FPolyglotTextData> TextReplacements;
};
