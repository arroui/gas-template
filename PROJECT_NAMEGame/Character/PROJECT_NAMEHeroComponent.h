// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/<PROJECT_NAME>PawnComponent.h"
#include "Camera/<PROJECT_NAME>CameraMode.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Input/<PROJECT_NAME>MappableConfigPair.h"
#include "<PROJECT_NAME>HeroComponent.generated.h"


class A<PROJECT_NAME>PlayerState;
class UInputComponent;
struct FInputActionValue;
class U<PROJECT_NAME>InputConfig;

/**
 * U<PROJECT_NAME>HeroComponent
 *
 *	A component used to create a player controlled pawns (characters, vehicles, etc..).
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class U<PROJECT_NAME>HeroComponent : public U<PROJECT_NAME>PawnComponent
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>HeroComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the hero component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "<PROJECT_NAME>|Hero")
	static U<PROJECT_NAME>HeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U<PROJECT_NAME>HeroComponent>() : nullptr); }

	void SetAbilityCameraMode(TSubclassOf<U<PROJECT_NAME>CameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	void AddAdditionalInputConfig(const U<PROJECT_NAME>InputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const U<PROJECT_NAME>InputConfig* InputConfig);

	/** True if this has completed OnPawnReadyToInitialize so is prepared for late-added features */
	bool HasPawnInitialized() const;

	/** True if this player has sent the BindInputsNow event and is prepared for bindings */
	bool IsReadyToBindInputs() const;

	static const FName NAME_BindInputsNow;

protected:

	virtual void OnRegister() override;

	virtual bool IsPawnComponentReadyToInitialize() const override;
	void OnPawnReadyToInitialize();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	TSubclassOf<U<PROJECT_NAME>CameraMode> DetermineCameraMode() const;
	
	void OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair);
	void OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair);

protected:

	/**
	 * Input Configs that should be added to this player when initalizing the input.
	 * 
	 * NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	 * If you do, then use the GameFeatureAction_AddInputConfig instead. 
	 */
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> DefaultInputConfigs;
	
	// Camera mode set by an ability.
	TSubclassOf<U<PROJECT_NAME>CameraMode> AbilityCameraMode;

	// Spec handle for the last ability to set a camera mode.
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	// True when the pawn has fully finished initialization
	bool bPawnHasInitialized;

	// True when player input bindings have been applyed, will never be true for non-players
	bool bReadyToBindInputs;
};
