// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameMode.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "System/<PROJECT_NAME>AssetManager.h"
#include "<PROJECT_NAME>GameState.h"
#include "System/<PROJECT_NAME>GameSession.h"
#include "Player/<PROJECT_NAME>PlayerController.h"
#include "Player/<PROJECT_NAME>PlayerBotController.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "Character/<PROJECT_NAME>Character.h"
#include "UI/<PROJECT_NAME>HUD.h"
#include "Character/<PROJECT_NAME>PawnExtensionComponent.h"
#include "Character/<PROJECT_NAME>PawnData.h"
#include "GameModes/<PROJECT_NAME>WorldSettings.h"
#include "GameModes/<PROJECT_NAME>ExperienceDefinition.h"
#include "GameModes/<PROJECT_NAME>ExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Development/<PROJECT_NAME>DeveloperSettings.h"
#include "Player/<PROJECT_NAME>PlayerSpawningManagerComponent.h"
#include "TimerManager.h"

A<PROJECT_NAME>GameMode::A<PROJECT_NAME>GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = A<PROJECT_NAME>GameState::StaticClass();
	GameSessionClass = A<PROJECT_NAME>GameSession::StaticClass();
	PlayerControllerClass = A<PROJECT_NAME>PlayerController::StaticClass();
	ReplaySpectatorPlayerControllerClass = A<PROJECT_NAME>ReplayPlayerController::StaticClass();
	PlayerStateClass = A<PROJECT_NAME>PlayerState::StaticClass();
	DefaultPawnClass = A<PROJECT_NAME>Character::StaticClass();
	HUDClass = A<PROJECT_NAME>HUD::StaticClass();
}

const U<PROJECT_NAME>PawnData* A<PROJECT_NAME>GameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = InController->GetPlayerState<A<PROJECT_NAME>PlayerState>())
		{
			if (const U<PROJECT_NAME>PawnData* PawnData = <PROJECT_NAME>PS->GetPawnData<U<PROJECT_NAME>PawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience
	check(GameState);
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const U<PROJECT_NAME>ExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience is loaded and there's still no pawn data, fall back to the default for now
		return U<PROJECT_NAME>AssetManager::Get().GetDefaultPawnData();
	}

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

void A<PROJECT_NAME>GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//@TODO: Eventually only do this for PIE/auto
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void A<PROJECT_NAME>GameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  - Matchmaking assignment (if present)
	//  - URL Options override
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Default experience

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(U<PROJECT_NAME>ExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = GetDefault<U<PROJECT_NAME>DeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (A<PROJECT_NAME>WorldSettings* TypedWorldSettings = Cast<A<PROJECT_NAME>WorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	U<PROJECT_NAME>AssetManager& AssetManager = U<PROJECT_NAME>AssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(Log<PROJECT_NAME>Experience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("<PROJECT_NAME>ExperienceDefinition"), FName("B_<PROJECT_NAME>DefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void A<PROJECT_NAME>GameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
#if WITH_SERVER_CODE
	if (ExperienceId.IsValid())
	{
		UE_LOG(Log<PROJECT_NAME>Experience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->ServerSetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Experience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
#endif
}

void A<PROJECT_NAME>GameMode::OnExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool A<PROJECT_NAME>GameMode::IsExperienceLoaded() const
{
	check(GameState);
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

UClass* A<PROJECT_NAME>GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const U<PROJECT_NAME>PawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* A<PROJECT_NAME>GameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (U<PROJECT_NAME>PawnExtensionComponent* PawnExtComp = U<PROJECT_NAME>PawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const U<PROJECT_NAME>PawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool A<PROJECT_NAME>GameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void A<PROJECT_NAME>GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* A<PROJECT_NAME>GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (U<PROJECT_NAME>PlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<U<PROJECT_NAME>PlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ChoosePlayerStart(Player);
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void A<PROJECT_NAME>GameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (U<PROJECT_NAME>PlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<U<PROJECT_NAME>PlayerSpawningManagerComponent>())
	{
		PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	}

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool A<PROJECT_NAME>GameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool A<PROJECT_NAME>GameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	if (U<PROJECT_NAME>PlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<U<PROJECT_NAME>PlayerSpawningManagerComponent>())
	{
		return PlayerSpawningComponent->ControllerCanRestart(Controller);
	}

	return true;
}

void A<PROJECT_NAME>GameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOn<PROJECT_NAME>ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void A<PROJECT_NAME>GameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	OnGameModeCombinedPostLoginDelegate.Broadcast(this, NewPlayer);
}

void A<PROJECT_NAME>GameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	else if (A<PROJECT_NAME>PlayerBotController* BotController = Cast<A<PROJECT_NAME>PlayerBotController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(BotController, &A<PROJECT_NAME>PlayerBotController::ServerRestartController);
	}
}

bool A<PROJECT_NAME>GameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void A<PROJECT_NAME>GameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(Log<PROJECT_NAME>, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}