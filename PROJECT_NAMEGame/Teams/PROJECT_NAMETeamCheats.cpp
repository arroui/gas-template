// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamCheats.h"
#include "<PROJECT_NAME>TeamSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>TeamCheats

void U<PROJECT_NAME>TeamCheats::CycleTeam()
{
	if (U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<U<PROJECT_NAME>TeamSubsystem>(GetWorld()))
	{
		APlayerController* PC = GetPlayerController();

		const int32 OldTeamId = TeamSubsystem->FindTeamFromObject(PC);
		const TArray<int32> TeamIds = TeamSubsystem->GetTeamIDs();
		
		if (TeamIds.Num())
		{
			const int32 IndexOfOldTeam = TeamIds.Find(OldTeamId);
			const int32 IndexToUse = (IndexOfOldTeam + 1) % TeamIds.Num();

			const int32 NewTeamId = TeamIds[IndexToUse];

			TeamSubsystem->ChangeTeamForActor(PC, NewTeamId);
		}

		const int32 ActualNewTeamId = TeamSubsystem->FindTeamFromObject(PC);

		UE_LOG(LogConsoleResponse, Log, TEXT("Changed to team %d (from team %d)"), ActualNewTeamId, OldTeamId);
	}
}

void U<PROJECT_NAME>TeamCheats::SetTeam(int32 TeamID)
{
	if (U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<U<PROJECT_NAME>TeamSubsystem>(GetWorld()))
	{
		if (TeamSubsystem->DoesTeamExist(TeamID))
		{
			APlayerController* PC = GetPlayerController();

			TeamSubsystem->ChangeTeamForActor(PC, TeamID);
		}
	}
}

void U<PROJECT_NAME>TeamCheats::ListTeams()
{
	if (U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<U<PROJECT_NAME>TeamSubsystem>(GetWorld()))
	{
		const TArray<int32> TeamIDs = TeamSubsystem->GetTeamIDs();

		for (const int32 TeamID : TeamIDs)
		{
			UE_LOG(LogConsoleResponse, Log, TEXT("Team ID %d"), TeamID);
		}
	}
}
