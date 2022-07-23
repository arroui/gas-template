// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamStatics.h"
#include "<PROJECT_NAME>TeamSubsystem.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "Engine/Engine.h"
#include "Teams/<PROJECT_NAME>TeamDisplayAsset.h"

//////////////////////////////////////////////////////////////////////

void U<PROJECT_NAME>TeamStatics::FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, U<PROJECT_NAME>TeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet)
{
	bIsPartOfTeam = false;
	TeamId = INDEX_NONE;
	DisplayAsset = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(Agent, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = World->GetSubsystem<U<PROJECT_NAME>TeamSubsystem>())
		{
			TeamId = TeamSubsystem->FindTeamFromObject(Agent);
			if (TeamId != INDEX_NONE)
			{
				bIsPartOfTeam = true;

				DisplayAsset = TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);

				if ((DisplayAsset == nullptr) && bLogIfNotSet)
				{
					UE_LOG(Log<PROJECT_NAME>Teams, Log, TEXT("FindTeamFromObject(%s) called too early (found team %d but no display asset set yet"), *GetPathNameSafe(Agent), TeamId);
				}
			}
		}
		else
		{
			UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("FindTeamFromObject(%s) failed: Team subsystem does not exist yet"), *GetPathNameSafe(Agent));
		}
	}
}

U<PROJECT_NAME>TeamDisplayAsset* U<PROJECT_NAME>TeamStatics::GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId)
{
	U<PROJECT_NAME>TeamDisplayAsset* Result = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (U<PROJECT_NAME>TeamSubsystem* TeamSubsystem = World->GetSubsystem<U<PROJECT_NAME>TeamSubsystem>())
		{
			return TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);
		}
	}
	return Result;
}

float U<PROJECT_NAME>TeamStatics::GetTeamScalarWithFallback(U<PROJECT_NAME>TeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue)
{
	if (DisplayAsset)
	{
		if (float* pValue = DisplayAsset->ScalarParameters.Find(ParameterName))
		{
			return *pValue;
		}
	}
	return DefaultValue;
}

FLinearColor U<PROJECT_NAME>TeamStatics::GetTeamColorWithFallback(U<PROJECT_NAME>TeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue)
{
	if (DisplayAsset)
	{
		if (FLinearColor* pColor = DisplayAsset->ColorParameters.Find(ParameterName))
		{
			return *pColor;
		}
	}
	return DefaultValue;
}

UTexture* U<PROJECT_NAME>TeamStatics::GetTeamTextureWithFallback(U<PROJECT_NAME>TeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue)
{
	if (DisplayAsset)
	{
		if (TObjectPtr<UTexture>* pTexture = DisplayAsset->TextureParameters.Find(ParameterName))
		{
			return *pTexture;
		}
	}
	return DefaultValue;
}
