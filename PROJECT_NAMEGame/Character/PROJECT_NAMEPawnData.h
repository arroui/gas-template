// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "<PROJECT_NAME>PawnData.generated.h"

class APawn;
class U<PROJECT_NAME>AbilitySet;
class U<PROJECT_NAME>InputConfig;
class U<PROJECT_NAME>AbilityTagRelationshipMapping;
class U<PROJECT_NAME>CameraMode;


/**
 * U<PROJECT_NAME>PawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "<PROJECT_NAME> Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class U<PROJECT_NAME>PawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>PawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from A<PROJECT_NAME>Pawn or A<PROJECT_NAME>Character).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Abilities")
	TArray<U<PROJECT_NAME>AbilitySet*> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Abilities")
	U<PROJECT_NAME>AbilityTagRelationshipMapping* TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Input")
	U<PROJECT_NAME>InputConfig* InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Camera")
	TSubclassOf<U<PROJECT_NAME>CameraMode> DefaultCameraMode;
};
