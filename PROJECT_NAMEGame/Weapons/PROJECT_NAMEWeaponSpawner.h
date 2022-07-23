// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "<PROJECT_NAME>WeaponSpawner.generated.h"

class U<PROJECT_NAME>InventoryItemDefinition;
class U<PROJECT_NAME>WeaponPickupDefinition;
class UCapsuleComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable,BlueprintType)
class <PROJECT_NAME_UPPER>GAME_API A<PROJECT_NAME>WeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	A<PROJECT_NAME>WeaponSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

protected:
	//Data asset used to configure a Weapon Spawner
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	U<PROJECT_NAME>WeaponPickupDefinition* WeaponDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_WeaponAvailability, Category = "<PROJECT_NAME>|WeaponPickup")
	bool bIsWeaponAvailable;

	//The amount of time between weapon pickup and weapon spawning in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	float CoolDownTime;

	//Delay between when the weapon is made available and when we check for a pawn standing in the spawner. Used to give the bIsWeaponAvailable OnRep time to fire and play FX. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	float CheckExistingOverlapDelay;

	//Used to drive weapon respawn time indicators 0-1
	UPROPERTY(BlueprintReadOnly, Transient, Category = "<PROJECT_NAME>|WeaponPickup")
	float CoolDownPercentage;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	UCapsuleComponent* CollisionVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	UStaticMeshComponent* PadMesh;

	UPROPERTY(BlueprintReadOnly, Category = "<PROJECT_NAME>|WeaponPickup")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "<PROJECT_NAME>|WeaponPickup")
	float WeaponMeshRotationSpeed;

	FTimerHandle CoolDownTimerHandle;

	FTimerHandle CheckOverlapsDelayTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	//Check for pawns standing on pad when the weapon is spawned. 
	void CheckForExistingOverlaps();

	UFUNCTION(BlueprintNativeEvent)
	void AttemptPickUpWeapon(APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "<PROJECT_NAME>|WeaponPickup")
	bool GiveWeapon(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);

	void StartCoolDown();

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|WeaponPickup")
	void ResetCoolDown();

	UFUNCTION()
	void OnCoolDownTimerComplete();

	void SetWeaponPickupVisibility(bool bShouldBeVisible);

	UFUNCTION(BlueprintNativeEvent, Category = "<PROJECT_NAME>|WeaponPickup")
	void PlayPickupEffects();

	UFUNCTION(BlueprintNativeEvent, Category = "<PROJECT_NAME>|WeaponPickup")
	void PlayRespawnEffects();

	UFUNCTION()
	void OnRep_WeaponAvailability();

	/** Searches an item definition type for a matching stat and returns the value, or 0 if not found */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "<PROJECT_NAME>|WeaponPickup")
	static int32 GetDefaultStatFromItemDef(const TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag);
};
