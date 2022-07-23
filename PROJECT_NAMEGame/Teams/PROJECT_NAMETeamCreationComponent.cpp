// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamCreationComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameModes/<PROJECT_NAME>ExperienceDefinition.h"
#include "GameModes/<PROJECT_NAME>ExperienceManagerComponent.h"
#include "<PROJECT_NAME>TeamPublicInfo.h"
#include "<PROJECT_NAME>TeamPrivateInfo.h"
#include "GameFramework/PlayerState.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"
#include "GameModes/<PROJECT_NAME>GameMode.h"

U<PROJECT_NAME>TeamCreationComponent::U<PROJECT_NAME>TeamCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PublicTeamInfoClass = A<PROJECT_NAME>TeamPublicInfo::StaticClass();
	PrivateTeamInfoClass = A<PROJECT_NAME>TeamPrivateInfo::StaticClass();
}

#if WITH_EDITOR
EDataValidationResult U<PROJECT_NAME>TeamCreationComponent::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);

	//@TODO: TEAMS: Validate that all display assets have the same properties set!

	return Result;
}
#endif

void U<PROJECT_NAME>TeamCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<U<PROJECT_NAME>ExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOn<PROJECT_NAME>ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void U<PROJECT_NAME>TeamCreationComponent::OnExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateTeams();
		ServerAssignPlayersToTeams();
	}
#endif
}

#if WITH_SERVER_CODE

void U<PROJECT_NAME>TeamCreationComponent::ServerCreateTeams()
{
	for (const auto& KVP : TeamsToCreate)
	{
		const int32 TeamId = KVP.Key;
		ServerCreateTeam(TeamId, KVP.Value);
	}
}

void U<PROJECT_NAME>TeamCreationComponent::ServerAssignPlayersToTeams()
{
	// Assign players that already exist to teams
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Cast<A<PROJECT_NAME>PlayerState>(PS))
		{
			ServerChooseTeamForPlayer(<PROJECT_NAME>PS);
		}
	}

	// Listen for new players logging in
	A<PROJECT_NAME>GameMode* GameMode = Cast<A<PROJECT_NAME>GameMode>(GameState->AuthorityGameMode);
	check(GameMode);

	GameMode->OnGameModeCombinedPostLogin().AddUObject(this, &ThisClass::OnPostLogin);
}

void U<PROJECT_NAME>TeamCreationComponent::ServerChooseTeamForPlayer(A<PROJECT_NAME>PlayerState* PS)
{
	if (PS->IsOnlyASpectator())
	{
		PS->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	else
	{
		const FGenericTeamId TeamID = IntegerToGenericTeamId(GetLeastPopulatedTeamID());
		PS->SetGenericTeamId(TeamID);
	}
}

void U<PROJECT_NAME>TeamCreationComponent::OnPostLogin(AGameModeBase* GameMode, AController* NewPlayer)
{
	check(NewPlayer);
	check(NewPlayer->PlayerState);
	if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Cast<A<PROJECT_NAME>PlayerState>(NewPlayer->PlayerState))
	{
		ServerChooseTeamForPlayer(<PROJECT_NAME>PS);
	}
}

void U<PROJECT_NAME>TeamCreationComponent::ServerCreateTeam(int32 TeamId, U<PROJECT_NAME>TeamDisplayAsset* DisplayAsset)
{
	check(HasAuthority());

	//@TODO: ensure the team doesn't already exist

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	A<PROJECT_NAME>TeamPublicInfo* NewTeamPublicInfo = World->SpawnActor<A<PROJECT_NAME>TeamPublicInfo>(PublicTeamInfoClass, SpawnInfo);
	checkf(NewTeamPublicInfo != nullptr, TEXT("Failed to create public team actor from class %s"), *GetPathNameSafe(*PublicTeamInfoClass));
	NewTeamPublicInfo->SetTeamId(TeamId);
	NewTeamPublicInfo->SetTeamDisplayAsset(DisplayAsset);

	A<PROJECT_NAME>TeamPrivateInfo* NewTeamPrivateInfo = World->SpawnActor<A<PROJECT_NAME>TeamPrivateInfo>(PrivateTeamInfoClass, SpawnInfo);
	checkf(NewTeamPrivateInfo != nullptr, TEXT("Failed to create private team actor from class %s"), *GetPathNameSafe(*PrivateTeamInfoClass));
	NewTeamPrivateInfo->SetTeamId(TeamId);
}

int32 U<PROJECT_NAME>TeamCreationComponent::GetLeastPopulatedTeamID() const
{
	const int32 NumTeams = TeamsToCreate.Num();
	if (NumTeams > 0)
	{
		TMap<int32, uint32> TeamMemberCounts;
		TeamMemberCounts.Reserve(NumTeams);

		for (const auto& KVP : TeamsToCreate)
		{
			const int32 TeamId = KVP.Key;
			TeamMemberCounts.Add(TeamId, 0);
		}

		AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Cast<A<PROJECT_NAME>PlayerState>(PS))
			{
				const int32 PlayerTeamID = <PROJECT_NAME>PS->GetTeamId();

				if ((PlayerTeamID != INDEX_NONE) && !<PROJECT_NAME>PS->IsInactive())	// do not count unassigned or disconnected players
				{
					check(TeamMemberCounts.Contains(PlayerTeamID))
					TeamMemberCounts[PlayerTeamID] += 1;
				}
			}
		}

		// sort by lowest team population, then by team ID
		int32 BestTeamId = INDEX_NONE;
		uint32 BestPlayerCount = TNumericLimits<uint32>::Max();
		for (const auto& KVP : TeamMemberCounts)
		{
			const int32 TestTeamId = KVP.Key;
			const uint32 TestTeamPlayerCount = KVP.Value;

			if (TestTeamPlayerCount < BestPlayerCount)
			{
				BestTeamId = TestTeamId;
				BestPlayerCount = TestTeamPlayerCount;
			}
			else if (TestTeamPlayerCount == BestPlayerCount)
			{
				if ((TestTeamId < BestTeamId) || (BestTeamId == INDEX_NONE))
				{
					BestTeamId = TestTeamId;
					BestPlayerCount = TestTeamPlayerCount;
				}
			}
		}

		return BestTeamId;
	}

	return INDEX_NONE;
}
#endif	// WITH_SERVER_CODE
