// Copyright Epic Games, Inc. All Rights Reserved.


#include "Input/<PROJECT_NAME>InputComboComponent.h"
#include "EnhancedInputComponent.h"
#include "<PROJECT_NAME>InputComponent.h"
#include "GameFramework/PlayerController.h"

U<PROJECT_NAME>InputComboComponent::U<PROJECT_NAME>InputComboComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void U<PROJECT_NAME>InputComboComponent::OnRegister()
{
	Super::OnRegister();
	
	BindInputCallbacks();
}

void U<PROJECT_NAME>InputComboComponent::OnUnregister()
{
	Super::OnUnregister();
	
	RemoveInputCallbacks();
}

void U<PROJECT_NAME>InputComboComponent::BindInputCallbacks()
{
	if (U<PROJECT_NAME>InputComponent* Input = GetInputComponent())
	{
		for (const FComboStep& Step : ComboSteps)
		{
			if (Step.Action)
			{
				Input->BindAction(Step.Action, ETriggerEvent::Triggered, this, &U<PROJECT_NAME>InputComboComponent::OnComboStepCompleted);
				// TODO: store handles
				BindHandles.Add(8);
			}
		}
	}
}

void U<PROJECT_NAME>InputComboComponent::RemoveInputCallbacks()
{
	if (U<PROJECT_NAME>InputComponent* Input = GetInputComponent())
	{
		Input->RemoveBinds(BindHandles);
	}
}

void U<PROJECT_NAME>InputComboComponent::OnComboStepCompleted(const FInputActionInstance& Instance)
{
	// It shouldn't be possible to have no combo steps
	ensure(!ComboSteps.IsEmpty());
	
	int32 CurrentComboStep = ComboStack.Num();
	const float TriggeredWorldTime = Instance.GetLastTriggeredWorldTime();	
	const float PreviousStepActivationTime = ComboStack[CurrentComboStep].Value;
	
	// If this is the first combo then we can just add it to the stack
	if (Instance.GetSourceAction() == ComboSteps[CurrentComboStep].Action)
	{
		if (TriggeredWorldTime - PreviousStepActivationTime <= ComboSteps[CurrentComboStep].CooldownTime)
		{
			// Check if the time is valid
			TTuple<const UInputAction*, float> NewlyCompletedStep;
			NewlyCompletedStep.Key = Instance.GetSourceAction();
			NewlyCompletedStep.Value = TriggeredWorldTime;
			ComboStack.Emplace(NewlyCompletedStep);	
		}
	}
	
	// Check if the combo was completed or not
	if (ComboStack.Num() == ComboSteps.Num())
	{
		NativeOnComboCompleted();
	}
}

void U<PROJECT_NAME>InputComboComponent::NativeOnComboCompleted()
{
	ComboStack.Empty();
	ScriptOnComboCompleted();
}

void U<PROJECT_NAME>InputComboComponent::NativeOnComboCanceled()
{
	ComboStack.Empty();
	ScriptOnComboCanceled();
}

U<PROJECT_NAME>InputComponent* U<PROJECT_NAME>InputComboComponent::GetInputComponent() const
{
	APlayerController* Owner = Cast<APlayerController>(GetOwner());
	return Owner ? Cast<U<PROJECT_NAME>InputComponent>(Owner->PlayerInput) : nullptr;
}

