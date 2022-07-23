// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamPublicInfo.h"
#include "Net/UnrealNetwork.h"

A<PROJECT_NAME>TeamPublicInfo::A<PROJECT_NAME>TeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void A<PROJECT_NAME>TeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void A<PROJECT_NAME>TeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<U<PROJECT_NAME>TeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void A<PROJECT_NAME>TeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}
