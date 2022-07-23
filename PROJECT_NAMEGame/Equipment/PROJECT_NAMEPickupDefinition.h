// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "<PROJECT_NAME>PickupDefinition.generated.h"

class U<PROJECT_NAME>InventoryItemDefinition;
class UStaticMesh;
class USoundBase;
class UNiagaraSystem;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "<PROJECT_NAME> Pickup Data", ShortTooltip = "Data asset used to configure a pickup."))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>PickupDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup|Equipment")
	TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> InventoryItemDefinition;

	//Visual representation of the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup|Mesh")
	UStaticMesh* DisplayMesh;

	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup")
	int32 SpawnCoolDownSeconds;

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup")
	USoundBase* PickedUpSound;

	//Sound to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup")
	USoundBase* RespawnedSound;

	//Particle FX to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup")
	UNiagaraSystem* PickedUpEffect;

	//Particle FX to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup")
	UNiagaraSystem* RespawnedEffect;
};


UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "<PROJECT_NAME> Weapon Pickup Data", ShortTooltip = "Data asset used to configure a weapon pickup."))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>WeaponPickupDefinition : public U<PROJECT_NAME>PickupDefinition
{
	GENERATED_BODY()

public:

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup|Mesh")
	FVector WeaponMeshOffset;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|Pickup|Mesh")
	FVector WeaponMeshScale = FVector(1.0f, 1.0f, 1.0f);
};