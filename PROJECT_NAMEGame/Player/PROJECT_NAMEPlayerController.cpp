// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PlayerController.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "GameModes/<PROJECT_NAME>GameMode.h"
#include "<PROJECT_NAME>CheatManager.h"
#include "<PROJECT_NAME>PlayerState.h"
#include "Camera/<PROJECT_NAME>PlayerCameraManager.h"
#include "UI/<PROJECT_NAME>HUD.h"
#include "Character/<PROJECT_NAME>PawnData.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemGlobals.h"
#include "CommonInputSubsystem.h"
#include "<PROJECT_NAME>LocalPlayer.h"
#include "Settings/<PROJECT_NAME>SettingsShared.h"
#include "Development/<PROJECT_NAME>DeveloperSettings.h"

A<PROJECT_NAME>PlayerController::A<PROJECT_NAME>PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = A<PROJECT_NAME>PlayerCameraManager::StaticClass();

#if USING_CHEAT_MANAGER
	CheatClass = U<PROJECT_NAME>CheatManager::StaticClass();
#endif // #if USING_CHEAT_MANAGER
}

void A<PROJECT_NAME>PlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A<PROJECT_NAME>PlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void A<PROJECT_NAME>PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void A<PROJECT_NAME>PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void A<PROJECT_NAME>PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	if (GetIsAutoRunning())
	{
		if (APawn* CurrentPawn = GetPawn())
		{
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);	
		}
	}
}

A<PROJECT_NAME>PlayerState* A<PROJECT_NAME>PlayerController::Get<PROJECT_NAME>PlayerState() const
{
	return CastChecked<A<PROJECT_NAME>PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

U<PROJECT_NAME>AbilitySystemComponent* A<PROJECT_NAME>PlayerController::Get<PROJECT_NAME>AbilitySystemComponent() const
{
	const A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Get<PROJECT_NAME>PlayerState();
	return (<PROJECT_NAME>PS ? <PROJECT_NAME>PS->Get<PROJECT_NAME>AbilitySystemComponent() : nullptr);
}

A<PROJECT_NAME>HUD* A<PROJECT_NAME>PlayerController::Get<PROJECT_NAME>HUD() const
{
	return CastChecked<A<PROJECT_NAME>HUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void A<PROJECT_NAME>PlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void A<PROJECT_NAME>PlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
}

void A<PROJECT_NAME>PlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState != nullptr)
	{
		if (I<PROJECT_NAME>TeamAgentInterface* PlayerStateTeamInterface = Cast<I<PROJECT_NAME>TeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		}
	}

	// Bind to the new player state, if any
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState != nullptr)
	{
		if (I<PROJECT_NAME>TeamAgentInterface* PlayerStateTeamInterface = Cast<I<PROJECT_NAME>TeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}

	// Broadcast the team change (if it really has)
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}

void A<PROJECT_NAME>PlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void A<PROJECT_NAME>PlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void A<PROJECT_NAME>PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

void A<PROJECT_NAME>PlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const U<PROJECT_NAME>LocalPlayer* <PROJECT_NAME>LocalPlayer = Cast<U<PROJECT_NAME>LocalPlayer>(InPlayer))
	{
		U<PROJECT_NAME>SettingsShared* UserSettings = <PROJECT_NAME>LocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void A<PROJECT_NAME>PlayerController::OnSettingsChanged(U<PROJECT_NAME>SettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}

void A<PROJECT_NAME>PlayerController::AddCheats(bool bForce)
{
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

void A<PROJECT_NAME>PlayerController::ServerCheat_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(Log<PROJECT_NAME>, Warning, TEXT("ServerCheat: %s"), *Msg);
		ClientMessage(ConsoleCommand(Msg));
	}
#endif // #if USING_CHEAT_MANAGER
}

bool A<PROJECT_NAME>PlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void A<PROJECT_NAME>PlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(Log<PROJECT_NAME>, Warning, TEXT("ServerCheatAll: %s"), *Msg);
		for (TActorIterator<A<PROJECT_NAME>PlayerController> It(GetWorld()); It; ++It)
		{
			A<PROJECT_NAME>PlayerController* <PROJECT_NAME>PC = (*It);
			if (<PROJECT_NAME>PC)
			{
				<PROJECT_NAME>PC->ClientMessage(<PROJECT_NAME>PC->ConsoleCommand(Msg));
			}
		}
	}
#endif // #if USING_CHEAT_MANAGER
}

bool A<PROJECT_NAME>PlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	return true;
}

void A<PROJECT_NAME>PlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void A<PROJECT_NAME>PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void A<PROJECT_NAME>PlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void A<PROJECT_NAME>PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

#if WITH_SERVER_CODE && WITH_EDITOR
	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
	{
		for (const F<PROJECT_NAME>CheatToRun& CheatRow : GetDefault<U<PROJECT_NAME>DeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
			{
				ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	SetIsAutoRunning(false);
}

void A<PROJECT_NAME>PlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool A<PROJECT_NAME>PlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		bIsAutoRunning = <PROJECT_NAME>ASC->GetTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void A<PROJECT_NAME>PlayerController::OnStartAutoRun()
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_AutoRunning, 1);
		K2_OnStartAutoRun();
	}	
}

void A<PROJECT_NAME>PlayerController::OnEndAutoRun()
{
	if (U<PROJECT_NAME>AbilitySystemComponent* <PROJECT_NAME>ASC = Get<PROJECT_NAME>AbilitySystemComponent())
	{
		<PROJECT_NAME>ASC->SetLooseGameplayTagCount(F<PROJECT_NAME>GameplayTags::Get().Status_AutoRunning, 0);
		K2_OnEndAutoRun();
	}
}

void A<PROJECT_NAME>PlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}
	
	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void A<PROJECT_NAME>PlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				for (UPrimitiveComponent* Comp : InComponents)
				{
					if (Comp->IsRegistered())
					{
						OutHiddenComponents.Add(Comp->ComponentId);

						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
							{
								OutHiddenComponents.Add(AttachChildPC->ComponentId);
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}

void A<PROJECT_NAME>PlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId A<PROJECT_NAME>PlayerController::GetGenericTeamId() const
{
	if (const I<PROJECT_NAME>TeamAgentInterface* PSWithTeamInterface = Cast<I<PROJECT_NAME>TeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOn<PROJECT_NAME>TeamIndexChangedDelegate* A<PROJECT_NAME>PlayerController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void A<PROJECT_NAME>PlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

//////////////////////////////////////////////////////////////////////
// A<PROJECT_NAME>ReplayPlayerController

void A<PROJECT_NAME>ReplayPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}
