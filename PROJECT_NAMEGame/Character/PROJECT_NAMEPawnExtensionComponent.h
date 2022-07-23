// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "<PROJECT_NAME>PawnComponent.h"
#include "<PROJECT_NAME>PawnExtensionComponent.generated.h"


class U<PROJECT_NAME>PawnData;
class U<PROJECT_NAME>AbilitySystemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(F<PROJECT_NAME>DynamicMulticastDelegate);



/**
 * U<PROJECT_NAME>PawnExtensionComponent
 *
 *	Component used to add functionality to all Pawn classes.
 */
UCLASS()
class U<PROJECT_NAME>PawnExtensionComponent : public U<PROJECT_NAME>PawnComponent
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>PawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the pawn extension component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "<PROJECT_NAME>|Pawn")
	static U<PROJECT_NAME>PawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U<PROJECT_NAME>PawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const U<PROJECT_NAME>PawnData* InPawnData);

	UFUNCTION(BlueprintPure, Category = "<PROJECT_NAME>|Pawn")
	U<PROJECT_NAME>AbilitySystemComponent* Get<PROJECT_NAME>AbilitySystemComponent() const { return AbilitySystemComponent; }

	// Should be called by the owning pawn to become the avatar of the ability system.
	void InitializeAbilitySystem(U<PROJECT_NAME>AbilitySystemComponent* InASC, AActor* InOwnerActor);

	// Should be called by the owning pawn to remove itself as the avatar of the ability system.
	void UninitializeAbilitySystem();

	// Should be called by the owning pawn when the pawn's controller changes.
	void HandleControllerChanged();

	// Should be called by the owning pawn when the player state has been replicated.
	void HandlePlayerStateReplicated();

	// Should be called by the owning pawn when the input component is setup.
	void SetupPlayerInputComponent();

	// Call this anytime the pawn needs to check if it's ready to be initialized (pawn data assigned, possessed, etc..). 
	bool CheckPawnReadyToInitialize();

	// Returns true if the pawn is ready to be initialized.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "<PROJECT_NAME>|Pawn", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsPawnReadyToInitialize() const { return bPawnReadyToInitialize; }

	// Register with the OnPawnReadyToInitialize delegate and broadcast if condition is already met.
	void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// Register with the OnAbilitySystemInitialized delegate and broadcast if condition is already met.
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	// Register with the OnAbilitySystemUninitialized delegate.
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:

	virtual void OnRegister() override;

	UFUNCTION()
	void OnRep_PawnData();

	// Delegate fired when pawn has everything needed for initialization.
	FSimpleMulticastDelegate OnPawnReadyToInitialize;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Pawn Ready To Initialize"))
	F<PROJECT_NAME>DynamicMulticastDelegate BP_OnPawnReadyToInitialize;

	// Delegate fired when our pawn becomes the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Delegate fired when our pawn is removed as the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

protected:

	// Pawn data used to create the pawn.  Specified from a spawn function or on a placed instance.
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "<PROJECT_NAME>|Pawn")
	const U<PROJECT_NAME>PawnData* PawnData;

	// Pointer to the ability system component that is cached for convenience.
	UPROPERTY()
	U<PROJECT_NAME>AbilitySystemComponent* AbilitySystemComponent;

	// True when the pawn has everything needed for initialization.
	int32 bPawnReadyToInitialize : 1;
};
