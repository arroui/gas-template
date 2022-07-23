// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"

#include "<PROJECT_NAME>CosmeticCheats.generated.h"

class U<PROJECT_NAME>ControllerComponent_CharacterParts;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class U<PROJECT_NAME>CosmeticCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>CosmeticCheats();

	// Adds a character part
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Replaces previous cheat parts with a new one
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Clears any existing cheats
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ClearCharacterPartOverrides();

private:
	U<PROJECT_NAME>ControllerComponent_CharacterParts* GetCosmeticComponent() const;
};
