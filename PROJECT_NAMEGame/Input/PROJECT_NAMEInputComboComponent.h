// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputAction.h"
#include "<PROJECT_NAME>InputComboComponent.generated.h"

USTRUCT(BlueprintType)
struct FComboStep
{
	GENERATED_BODY()

	const UInputAction* Action = nullptr;

	/**
	 * The amount of time that is allowed to pass between when this step is triggered and the next one.
	 * If this 
	 * 
	 */
	float CooldownTime = 0.5f;
};

class U<PROJECT_NAME>InputComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>InputComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	U<PROJECT_NAME>InputComboComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

protected:

	void OnComboStepCompleted(const FInputActionInstance& Instance);
	
	/** Called when the combo has been completed! */
	void NativeOnComboCompleted();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Combo Completed"))
	void ScriptOnComboCompleted();

	void NativeOnComboCanceled();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Combo Canceled"))
	void ScriptOnComboCanceled();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FComboStep> ComboSteps;

	U<PROJECT_NAME>InputComponent* GetInputComponent() const;

private:

	void BindInputCallbacks();
	void RemoveInputCallbacks();
	
	/** The stack of the completed combo steps  */
	TArray<TTuple<const UInputAction*, float>> ComboStack;

	TArray<uint32> BindHandles;
};
