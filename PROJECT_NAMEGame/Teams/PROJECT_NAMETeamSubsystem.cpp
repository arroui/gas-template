// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "<PROJECT_NAME>TeamPublicInfo.h"
#include "<PROJECT_NAME>TeamPrivateInfo.h"
#include "GameFramework/Controller.h"
#include "Player/<PROJECT_NAME>PlayerState.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemGlobals.h"
#include "<PROJECT_NAME>TeamCheats.h"
#include "<PROJECT_NAME>TeamAgentInterface.h"
#include "<PROJECT_NAME>LogChannels.h"

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>TeamTrackingInfo

void F<PROJECT_NAME>TeamTrackingInfo::SetTeamInfo(A<PROJECT_NAME>TeamInfoBase* Info)
{
	if (A<PROJECT_NAME>TeamPublicInfo* NewPublicInfo = Cast<A<PROJECT_NAME>TeamPublicInfo>(Info))
	{
		ensure((PublicInfo == nullptr) || (PublicInfo == NewPublicInfo));
		PublicInfo = NewPublicInfo;

		U<PROJECT_NAME>TeamDisplayAsset* OldDisplayAsset = DisplayAsset;
		DisplayAsset = NewPublicInfo->GetTeamDisplayAsset();

		if (OldDisplayAsset != DisplayAsset)
		{
			OnTeamDisplayAssetChanged.Broadcast(DisplayAsset);
		}
	}
	else if (A<PROJECT_NAME>TeamPrivateInfo* NewPrivateInfo = Cast<A<PROJECT_NAME>TeamPrivateInfo>(Info))
	{
		ensure((PrivateInfo == nullptr) || (PrivateInfo == NewPrivateInfo));
		PrivateInfo = NewPrivateInfo;
	}
	else
	{
		checkf(false, TEXT("Expected a public or private team info but got %s"), *GetPathNameSafe(Info))
	}
}

void F<PROJECT_NAME>TeamTrackingInfo::RemoveTeamInfo(A<PROJECT_NAME>TeamInfoBase* Info)
{
	if (PublicInfo == Info)
	{
		PublicInfo = nullptr;
	}
	else if (PrivateInfo == Info)
	{
		PrivateInfo = nullptr;
	}
	else
	{
		ensureMsgf(false, TEXT("Expected a previously registered team info but got %s"), *GetPathNameSafe(Info));
	}
}

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>TeamSubsystem

U<PROJECT_NAME>TeamSubsystem::U<PROJECT_NAME>TeamSubsystem()
{
}

void U<PROJECT_NAME>TeamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	auto AddTeamCheats = [](UCheatManager* CheatManager)
	{
		CheatManager->AddCheatManagerExtension(NewObject<U<PROJECT_NAME>TeamCheats>(CheatManager));
	};

	CheatManagerRegistrationHandle = UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(AddTeamCheats));
}

void U<PROJECT_NAME>TeamSubsystem::Deinitialize()
{
	UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerRegistrationHandle);

	Super::Deinitialize();
}

void U<PROJECT_NAME>TeamSubsystem::RegisterTeamInfo(A<PROJECT_NAME>TeamInfoBase* TeamInfo)
{
	check(TeamInfo);
	const int32 TeamId = TeamInfo->GetTeamId();
	check(TeamId != INDEX_NONE);

	F<PROJECT_NAME>TeamTrackingInfo& Entry = TeamMap.FindOrAdd(TeamId);
	Entry.SetTeamInfo(TeamInfo);
}

void U<PROJECT_NAME>TeamSubsystem::UnregisterTeamInfo(A<PROJECT_NAME>TeamInfoBase* TeamInfo)
{
	check(TeamInfo);
	const int32 TeamId = TeamInfo->GetTeamId();
	check(TeamId != INDEX_NONE);

	F<PROJECT_NAME>TeamTrackingInfo& Entry = TeamMap.FindChecked(TeamId);
	Entry.RemoveTeamInfo(TeamInfo);
}

bool U<PROJECT_NAME>TeamSubsystem::ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamIndex)
{
	const FGenericTeamId NewTeamID = IntegerToGenericTeamId(NewTeamIndex);
	if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = const_cast<A<PROJECT_NAME>PlayerState*>(FindPlayerStateFromActor(ActorToChange)))
	{
		<PROJECT_NAME>PS->SetGenericTeamId(NewTeamID);
		return true;
	}
	else if (I<PROJECT_NAME>TeamAgentInterface* TeamActor = Cast<I<PROJECT_NAME>TeamAgentInterface>(ActorToChange))
	{
		TeamActor->SetGenericTeamId(NewTeamID);
		return true;
	}
	else
	{
		return false;
	}
}

int32 U<PROJECT_NAME>TeamSubsystem::FindTeamFromObject(const UObject* TestObject) const
{
	// See if it's directly a team agent
	if (const I<PROJECT_NAME>TeamAgentInterface* ObjectWithTeamInterface = Cast<I<PROJECT_NAME>TeamAgentInterface>(TestObject))
	{
		return GenericTeamIdToInteger(ObjectWithTeamInterface->GetGenericTeamId());
	}

	if (const AActor* TestActor = Cast<const AActor>(TestObject))
	{
		// See if the instigator is a team actor
		if (const I<PROJECT_NAME>TeamAgentInterface* InstigatorWithTeamInterface = Cast<I<PROJECT_NAME>TeamAgentInterface>(TestActor->GetInstigator()))
		{
			return GenericTeamIdToInteger(InstigatorWithTeamInterface->GetGenericTeamId());
		}

		// Fall back to finding the associated player state
		if (const A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = FindPlayerStateFromActor(TestActor))
		{
			return <PROJECT_NAME>PS->GetTeamId();
		}
	}

	return INDEX_NONE;
}

const A<PROJECT_NAME>PlayerState* U<PROJECT_NAME>TeamSubsystem::FindPlayerStateFromActor(const AActor* PossibleTeamActor) const
{
	if (PossibleTeamActor != nullptr)
	{
		if (const APawn* Pawn = Cast<const APawn>(PossibleTeamActor))
		{
			//@TODO: Consider an interface instead or have team actors register with the subsystem and have it maintain a map? (or LWC style)
			if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Pawn->GetPlayerState<A<PROJECT_NAME>PlayerState>())
			{
				return <PROJECT_NAME>PS;
			}
		}
		else if (const AController* PC = Cast<const AController>(PossibleTeamActor))
		{
			if (A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Cast<A<PROJECT_NAME>PlayerState>(PC->PlayerState))
			{
				return <PROJECT_NAME>PS;
			}
		}
		else if (const A<PROJECT_NAME>PlayerState* <PROJECT_NAME>PS = Cast<const A<PROJECT_NAME>PlayerState>(PossibleTeamActor))
		{
			return <PROJECT_NAME>PS; 
		}

		// Try the instigator
// 		if (AActor* Instigator = PossibleTeamActor->GetInstigator())
// 		{
// 			if (ensure(Instigator != PossibleTeamActor))
// 			{
// 				return FindPlayerStateFromActor(Instigator);
// 			}
// 		}
	}

	return nullptr;
}

E<PROJECT_NAME>TeamComparison U<PROJECT_NAME>TeamSubsystem::CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const
{
	TeamIdA = FindTeamFromObject(Cast<const AActor>(A));
	TeamIdB = FindTeamFromObject(Cast<const AActor>(B));

	if ((TeamIdA == INDEX_NONE) || (TeamIdB == INDEX_NONE))
	{
		return E<PROJECT_NAME>TeamComparison::InvalidArgument;
	}
	else
	{
		return (TeamIdA == TeamIdB) ? E<PROJECT_NAME>TeamComparison::OnSameTeam : E<PROJECT_NAME>TeamComparison::DifferentTeams;
	}
}

E<PROJECT_NAME>TeamComparison U<PROJECT_NAME>TeamSubsystem::CompareTeams(const UObject* A, const UObject* B) const
{
	int32 TeamIdA;
	int32 TeamIdB;
	return CompareTeams(A, B, /*out*/ TeamIdA, /*out*/ TeamIdB);
}

void U<PROJECT_NAME>TeamSubsystem::FindTeamFromActor(const UObject* TestObject, bool& bIsPartOfTeam, int32& TeamId) const
{
	TeamId = FindTeamFromObject(TestObject);
	bIsPartOfTeam = TeamId != INDEX_NONE;
}

void U<PROJECT_NAME>TeamSubsystem::AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("AddTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (F<PROJECT_NAME>TeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.AddStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void U<PROJECT_NAME>TeamSubsystem::RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Error, TEXT("RemoveTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (F<PROJECT_NAME>TeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.RemoveStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

int32 U<PROJECT_NAME>TeamSubsystem::GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const
{
	if (const F<PROJECT_NAME>TeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		const int32 PublicStackCount = (Entry->PublicInfo != nullptr) ? Entry->PublicInfo->TeamTags.GetStackCount(Tag) : 0;
		const int32 PrivateStackCount = (Entry->PrivateInfo != nullptr) ? Entry->PrivateInfo->TeamTags.GetStackCount(Tag) : 0;
		return PublicStackCount + PrivateStackCount;
	}
	else
	{
		UE_LOG(Log<PROJECT_NAME>Teams, Verbose, TEXT("GetTeamTagStackCount(TeamId: %d, Tag: %s) failed because it was passed an unknown team id"), TeamId, *Tag.ToString());
		return 0;
	}
}

bool U<PROJECT_NAME>TeamSubsystem::TeamHasTag(int32 TeamId, FGameplayTag Tag) const
{
	return GetTeamTagStackCount(TeamId, Tag) > 0;
}

bool U<PROJECT_NAME>TeamSubsystem::DoesTeamExist(int32 TeamId) const
{
	return TeamMap.Contains(TeamId);
}

TArray<int32> U<PROJECT_NAME>TeamSubsystem::GetTeamIDs() const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);
	Result.Sort();
	return Result;
}

bool U<PROJECT_NAME>TeamSubsystem::CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf) const
{
	if (bAllowDamageToSelf)
	{
		if ((Instigator == Target) || (FindPlayerStateFromActor(Cast<AActor>(Instigator)) == FindPlayerStateFromActor(Cast<AActor>(Target))))
		{
			return true;
		}
	}

	int32 InstigatorTeamId;
	int32 TargetTeamId;
	const E<PROJECT_NAME>TeamComparison Relationship = CompareTeams(Instigator, Target, /*out*/ InstigatorTeamId, /*out*/ TargetTeamId);
	if (Relationship == E<PROJECT_NAME>TeamComparison::DifferentTeams)
	{
		return true;
	}
	else if ((Relationship == E<PROJECT_NAME>TeamComparison::InvalidArgument) && (InstigatorTeamId != INDEX_NONE))
	{
		// Allow damaging non-team actors for now, as long as they have an ability system component
		//@TODO: This is temporary until the target practice dummy has a team assignment
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Cast<const AActor>(Target)) != nullptr;
	}

	return false;
}

U<PROJECT_NAME>TeamDisplayAsset* U<PROJECT_NAME>TeamSubsystem::GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId)
{
	// Currently ignoring ViewerTeamId

	if (F<PROJECT_NAME>TeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		return Entry->DisplayAsset;
	}

	return nullptr;
}

U<PROJECT_NAME>TeamDisplayAsset* U<PROJECT_NAME>TeamSubsystem::GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent)
{
	return GetTeamDisplayAsset(TeamId, FindTeamFromObject(ViewerTeamAgent));
}

void U<PROJECT_NAME>TeamSubsystem::NotifyTeamDisplayAssetModified(U<PROJECT_NAME>TeamDisplayAsset* /*ModifiedAsset*/)
{
	// Broadcasting to all observers when a display asset is edited right now, instead of only the edited one
	for (const auto& KVP : TeamMap)
	{
		const int32 TeamId = KVP.Key;
		const F<PROJECT_NAME>TeamTrackingInfo& TrackingInfo = KVP.Value;

		TrackingInfo.OnTeamDisplayAssetChanged.Broadcast(TrackingInfo.DisplayAsset);
	}
}

FOn<PROJECT_NAME>TeamDisplayAssetChangedDelegate& U<PROJECT_NAME>TeamSubsystem::GetTeamDisplayAssetChangedDelegate(int32 TeamId)
{
	return TeamMap.FindOrAdd(TeamId).OnTeamDisplayAssetChanged;
}
