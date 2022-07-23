// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "<PROJECT_NAME>CharacterPartTypes.h"

#include "<PROJECT_NAME>ControllerComponent_CharacterParts.generated.h"

class U<PROJECT_NAME>PawnComponent_CharacterParts;

enum class ECharacterPartSource : uint8
{
	Natural,

	NaturalSuppressedViaCheat,

	AppliedViaDeveloperSettingsCheat,

	AppliedViaCheatManager
};

//////////////////////////////////////////////////////////////////////

// A character part requested on a controller component
USTRUCT()
struct F<PROJECT_NAME>ControllerCharacterPartEntry
{
	GENERATED_BODY()

	F<PROJECT_NAME>ControllerCharacterPartEntry()
	{}

public:
	// The character part being represented
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	F<PROJECT_NAME>CharacterPart Part;

	// The handle if already applied to a pawn
	F<PROJECT_NAME>CharacterPartHandle Handle;

	// The source of this part
	ECharacterPartSource Source = ECharacterPartSource::Natural;
};

//////////////////////////////////////////////////////////////////////

// A component that configure what cosmetic actors to spawn for the owning controller when it possesses a pawn
UCLASS(meta = (BlueprintSpawnableComponent))
class U<PROJECT_NAME>ControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>ControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void AddCharacterPart(const F<PROJECT_NAME>CharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(const F<PROJECT_NAME>CharacterPart& PartToRemove);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Applies relevant developer settings if in PIE
	void ApplyDeveloperSettings();

protected:
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	TArray<F<PROJECT_NAME>ControllerCharacterPartEntry> CharacterParts;

private:
	U<PROJECT_NAME>PawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void AddCharacterPartInternal(const F<PROJECT_NAME>CharacterPart& NewPart, ECharacterPartSource Source);

	void AddCheatPart(const F<PROJECT_NAME>CharacterPart& NewPart, bool bSuppressNaturalParts);
	void ClearCheatParts();

	void SetSuppressionOnNaturalParts(bool bSuppressed);

	friend class U<PROJECT_NAME>CosmeticCheats;
};