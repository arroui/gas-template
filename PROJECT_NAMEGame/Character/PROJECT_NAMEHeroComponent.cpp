// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>HeroComponent.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/<PROJECT_NAME>PlayerController.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "Character/<PROJECT_NAME>PawnData.h"
#include "Character/<PROJECT_NAME>Character.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "Input/<PROJECT_NAME>InputConfig.h"
#include "Input/<PROJECT_NAME>InputComponent.h"
#include "Camera/<PROJECT_NAME>CameraComponent.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "Engine/LocalPlayer.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "PlayerMappableInputConfig.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace <PROJECT_NAME>Hero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName U<PROJECT_NAME>HeroComponent::NAME_BindInputsNow("BindInputsNow");

U<PROJECT_NAME>HeroComponent::U<PROJECT_NAME>HeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bPawnHasInitialized = false;
	bReadyToBindInputs = false;
}

void U<PROJECT_NAME>HeroComponent::OnRegister()
{
	Super::OnRegister();

	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("[U<PROJECT_NAME>HeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("<PROJECT_NAME>HeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("<PROJECT_NAME>HeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
}

bool U<PROJECT_NAME>HeroComponent::IsPawnComponentReadyToInitialize() const
{
	// The player state is required.
	if (!GetPlayerState<A<PROJECT_NAME>PlayerState>())
	{
		return false;
	}

	const APawn* Pawn = GetPawn<APawn>();

	// A pawn is required.
	if (!Pawn)
	{
		return false;
	}

	// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
	if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
	{
		AController* Controller = GetController<AController>();

		const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
												(Controller->PlayerState != nullptr) && \
												(Controller->PlayerState->GetOwner() == Controller);

		if (!bHasControllerPairedWithPS)
		{
			return false;
		}
	}
	
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
	const bool bIsBot = Pawn->IsBotControlled();

	if (bIsLocallyControlled && !bIsBot)
	{
		// The input component is required when locally controlled.
		if (!Pawn->InputComponent)
		{
			return false;
		}
	}

	return true;
}

void U<PROJECT_NAME>HeroComponent::OnPawnReadyToInitialize()
{
	if (!ensure(!bPawnHasInitialized))
	{
		// Don't initialize twice
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = GetPlayerState<A<PROJECT_NAME>PlayerState>();
	check(<PROJECT_NAME>PS);

	const U<PROJECT_NAME>PawnData* PawnData = nullptr;

	if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtComp->GetPawnData<U<PROJECT_NAME>PawnData>();

		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		PawnExtComp->InitializeAbilitySystem(<PROJECT_NAME>PS->Get<PROJECT_NAME>AbilitySystemComponent(), <PROJECT_NAME>PS);
	}

	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = GetController<A<PROJECT_NAME>PlayerController>())
	{
		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	if (bIsLocallyControlled && PawnData)
	{
		if (U<PROJECT_NAME>CameraComponent* CameraComponent = U<PROJECT_NAME>CameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}

	bPawnHasInitialized = true;
}

void U<PROJECT_NAME>HeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void U<PROJECT_NAME>HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->UninitializeAbilitySystem();
		}	
	}

	Super::EndPlay(EndPlayReason);
}

void U<PROJECT_NAME>HeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const U<PROJECT_NAME>PawnData* PawnData = PawnExtComp->GetPawnData<U<PROJECT_NAME>PawnData>())
		{
			if (const U<PROJECT_NAME>InputConfig* InputConfig = PawnData->InputConfig)
			{
				const F<PROJECT_NAME>GameplayTags& GameplayTags = F<PROJECT_NAME>GameplayTags::Get();
	
				// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				{
					FMappableConfigPair::ActivatePair(Pair);
				}
				
				U<PROJECT_NAME>InputComponent* <PROJECT_NAME>IC = CastChecked<U<PROJECT_NAME>InputComponent>(PlayerInputComponent);
				<PROJECT_NAME>IC->AddInputMappings(InputConfig, Subsystem);
				if (U<PROJECT_NAME>SettingsLocal* LocalSettings = U<PROJECT_NAME>SettingsLocal::Get())
				{
					LocalSettings->OnInputConfigActivated.AddUObject(this, &U<PROJECT_NAME>HeroComponent::OnInputConfigActivated);
					LocalSettings->OnInputConfigDeactivated.AddUObject(this, &U<PROJECT_NAME>HeroComponent::OnInputConfigDeactivated);
				}

				TArray<uint32> BindHandles;
				<PROJECT_NAME>IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				<PROJECT_NAME>IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				<PROJECT_NAME>IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				<PROJECT_NAME>IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				<PROJECT_NAME>IC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				<PROJECT_NAME>IC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void U<PROJECT_NAME>HeroComponent::OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = GetController<A<PROJECT_NAME>PlayerController>())
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (U<PROJECT_NAME>InputComponent* <PROJECT_NAME>IC = Cast<U<PROJECT_NAME>InputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = <PROJECT_NAME>PC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						<PROJECT_NAME>IC->AddInputConfig(ConfigPair, Subsystem);	
					}	
				}
			}
		}
	}
}

void U<PROJECT_NAME>HeroComponent::OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = GetController<A<PROJECT_NAME>PlayerController>())
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (U<PROJECT_NAME>InputComponent* <PROJECT_NAME>IC = Cast<U<PROJECT_NAME>InputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = <PROJECT_NAME>PC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						<PROJECT_NAME>IC->RemoveInputConfig(ConfigPair, Subsystem);
					}
				}
			}
		}
	}
}

void U<PROJECT_NAME>HeroComponent::AddAdditionalInputConfig(const U<PROJECT_NAME>InputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	U<PROJECT_NAME>InputComponent* <PROJECT_NAME>IC = Pawn->FindComponentByClass<U<PROJECT_NAME>InputComponent>();
	check(<PROJECT_NAME>IC);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		<PROJECT_NAME>IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void U<PROJECT_NAME>HeroComponent::RemoveAdditionalInputConfig(const U<PROJECT_NAME>InputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool U<PROJECT_NAME>HeroComponent::HasPawnInitialized() const
{
	return bPawnHasInitialized;
}

bool U<PROJECT_NAME>HeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void U<PROJECT_NAME>HeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = PawnExtComp->Get<PROJECT_NAME>AbilitySystemComponent())
			{
				<PROJECT_NAME>ASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void U<PROJECT_NAME>HeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = PawnExtComp->Get<PROJECT_NAME>AbilitySystemComponent())
		{
			<PROJECT_NAME>ASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void U<PROJECT_NAME>HeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (A<PROJECT_NAME>PlayerController* <PROJECT_NAME>Controller = Cast<A<PROJECT_NAME>PlayerController>(Controller))
	{
		<PROJECT_NAME>Controller->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void U<PROJECT_NAME>HeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void U<PROJECT_NAME>HeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * <PROJECT_NAME>Hero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * <PROJECT_NAME>Hero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void U<PROJECT_NAME>HeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (A<PROJECT_NAME>Character* Character = GetPawn<A<PROJECT_NAME>Character>())
	{
		Character->ToggleCrouch();
	}
}

void U<PROJECT_NAME>HeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (A<PROJECT_NAME>PlayerController* Controller = Cast<A<PROJECT_NAME>PlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}	
	}
}

TSubclassOf<U<PROJECT_NAME>CameraMode> U<PROJECT_NAME>HeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const U<PROJECT_NAME>PawnData* PawnData = PawnExtComp->GetPawnData<U<PROJECT_NAME>PawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void U<PROJECT_NAME>HeroComponent::SetAbilityCameraMode(TSubclassOf<U<PROJECT_NAME>CameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void U<PROJECT_NAME>HeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}
