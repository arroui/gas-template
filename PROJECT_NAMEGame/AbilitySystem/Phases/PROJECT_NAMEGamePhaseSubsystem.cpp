// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GamePhaseSubsystem.h"
#include "<PROJECT_NAME>GamePhaseAbility.h"
#include "GameplayTagsManager.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/<PROJECT_NAME>GameplayAbility.h"
#include "AbilitySystem/<PROJECT_NAME>AbilitySystemComponent.h"
#include "<PROJECT_NAME>GamePhaseLog.h"

DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>GamePhase);

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>GamePhaseSubsystem

U<PROJECT_NAME>GamePhaseSubsystem::U<PROJECT_NAME>GamePhaseSubsystem()
{
}

void U<PROJECT_NAME>GamePhaseSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

bool U<PROJECT_NAME>GamePhaseSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Super::ShouldCreateSubsystem(Outer))
	{
		//UWorld* World = Cast<UWorld>(Outer);
		//check(World);

		//return World->GetAuthGameMode() != nullptr;
		//return nullptr;
		return true;
	}

	return false;
}

bool U<PROJECT_NAME>GamePhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void U<PROJECT_NAME>GamePhaseSubsystem::StartPhase(TSubclassOf<U<PROJECT_NAME>GamePhaseAbility> PhaseAbility, F<PROJECT_NAME>GamePhaseDelegate PhaseEndedCallback)
{
	UWorld* World = GetWorld();
	U<PROJECT_NAME>AbilitySystemComponent* GameState_ASC = World->GetGameState()->FindComponentByClass<U<PROJECT_NAME>AbilitySystemComponent>();
	if (ensure(GameState_ASC))
	{
		FGameplayAbilitySpec PhaseSpec(PhaseAbility, 1, 0, this);
		FGameplayAbilitySpecHandle SpecHandle = GameState_ASC->GiveAbilityAndActivateOnce(PhaseSpec);
		FGameplayAbilitySpec* FoundSpec = GameState_ASC->FindAbilitySpecFromHandle(SpecHandle);
		
		if (FoundSpec && FoundSpec->IsActive())
		{
			F<PROJECT_NAME>GamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(SpecHandle);
			Entry.PhaseEndedCallback = PhaseEndedCallback;
		}
		else
		{
			PhaseEndedCallback.ExecuteIfBound(nullptr);
		}
	}
}

void U<PROJECT_NAME>GamePhaseSubsystem::K2_StartPhase(TSubclassOf<U<PROJECT_NAME>GamePhaseAbility> PhaseAbility, const F<PROJECT_NAME>GamePhaseDynamicDelegate& PhaseEndedDelegate)
{
	const F<PROJECT_NAME>GamePhaseDelegate EndedDelegate = F<PROJECT_NAME>GamePhaseDelegate::CreateWeakLambda(const_cast<UObject*>(PhaseEndedDelegate.GetUObject()), [PhaseEndedDelegate](const U<PROJECT_NAME>GamePhaseAbility* PhaseAbility) {
		PhaseEndedDelegate.ExecuteIfBound(PhaseAbility);
	});

	StartPhase(PhaseAbility, EndedDelegate);
}

void U<PROJECT_NAME>GamePhaseSubsystem::K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, F<PROJECT_NAME>GamePhaseTagDynamicDelegate WhenPhaseActive)
{
	const F<PROJECT_NAME>GamePhaseTagDelegate ActiveDelegate = F<PROJECT_NAME>GamePhaseTagDelegate::CreateWeakLambda(WhenPhaseActive.GetUObject(), [WhenPhaseActive](const FGameplayTag& PhaseTag) {
		WhenPhaseActive.ExecuteIfBound(PhaseTag);
	});

	WhenPhaseStartsOrIsActive(PhaseTag, MatchType, ActiveDelegate);
}

void U<PROJECT_NAME>GamePhaseSubsystem::K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, F<PROJECT_NAME>GamePhaseTagDynamicDelegate WhenPhaseEnd)
{
	const F<PROJECT_NAME>GamePhaseTagDelegate EndedDelegate = F<PROJECT_NAME>GamePhaseTagDelegate::CreateWeakLambda(WhenPhaseEnd.GetUObject(), [WhenPhaseEnd](const FGameplayTag& PhaseTag) {
		WhenPhaseEnd.ExecuteIfBound(PhaseTag);
	});

	WhenPhaseEnds(PhaseTag, MatchType, EndedDelegate);
}

void U<PROJECT_NAME>GamePhaseSubsystem::WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const F<PROJECT_NAME>GamePhaseTagDelegate& WhenPhaseActive)
{
	FPhaseObserver Observer;
	Observer.PhaseTag = PhaseTag;
	Observer.MatchType = MatchType;
	Observer.PhaseCallback = WhenPhaseActive;
	PhaseStartObservers.Add(Observer);

	if (IsPhaseActive(PhaseTag))
	{
		WhenPhaseActive.ExecuteIfBound(PhaseTag);
	}
}

void U<PROJECT_NAME>GamePhaseSubsystem::WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const F<PROJECT_NAME>GamePhaseTagDelegate& WhenPhaseEnd)
{
	FPhaseObserver Observer;
	Observer.PhaseTag = PhaseTag;
	Observer.MatchType = MatchType;
	Observer.PhaseCallback = WhenPhaseEnd;
	PhaseEndObservers.Add(Observer);
}

bool U<PROJECT_NAME>GamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
	for (const auto& KVP : ActivePhaseMap)
	{
		const F<PROJECT_NAME>GamePhaseEntry& PhaseEntry = KVP.Value;
		if (PhaseEntry.PhaseTag.MatchesTag(PhaseTag))
		{
			return true;
		}
	}

	return false;
}

void U<PROJECT_NAME>GamePhaseSubsystem::OnBeginPhase(const U<PROJECT_NAME>GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	const FGameplayTag IncomingPhaseTag = PhaseAbility->GetGamePhaseTag();
	const FGameplayTag IncomingPhaseParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(IncomingPhaseTag);

	UE_LOG(Log<PROJECT_NAME>GamePhase, Log, TEXT("Beginning Phase '%s' (%s)"), *IncomingPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

	const UWorld* World = GetWorld();
	U<PROJECT_NAME>AbilitySystemComponent* GameState_ASC = World->GetGameState()->FindComponentByClass<U<PROJECT_NAME>AbilitySystemComponent>();
	if (ensure(GameState_ASC))
	{
		TArray<FGameplayAbilitySpec*> ActivePhases;
		for (const auto& KVP : ActivePhaseMap)
		{
			const FGameplayAbilitySpecHandle ActiveAbilityHandle = KVP.Key;
			if (FGameplayAbilitySpec* Spec = GameState_ASC->FindAbilitySpecFromHandle(ActiveAbilityHandle))
			{
				ActivePhases.Add(Spec);
			}
		}

		for (const FGameplayAbilitySpec* ActivePhase : ActivePhases)
		{
			const U<PROJECT_NAME>GamePhaseAbility* ActivePhaseAbility = CastChecked<U<PROJECT_NAME>GamePhaseAbility>(ActivePhase->Ability);
			const FGameplayTag ActivePhaseTag = ActivePhaseAbility->GetGamePhaseTag();
			
			// So if the active phase currently matches the incoming phase tag, we allow it.
			// i.e. multiple gameplay abilities can all be associated with the same phase tag.
			// For example,
			// You can be in the, Game.Playing, phase, and then start a sub-phase, like Game.Playing.SuddenDeath
			// Game.Playing phase will still be active, and if someone were to push another one, like,
			// Game.Playing.ActualSuddenDeath, it would end Game.Playing.SuddenDeath phase, but Game.Playing would
			// continue.  Similarly if we activated Game.GameOver, all the Game.Playing* phases would end.
			if (!ActivePhaseTag.MatchesTag(IncomingPhaseTag) && ActivePhaseTag.MatchesTag(IncomingPhaseParentTag))
			{
				UE_LOG(Log<PROJECT_NAME>GamePhase, Log, TEXT("\tEnding Phase '%s' (%s)"), *ActivePhaseTag.ToString(), *GetNameSafe(ActivePhaseAbility));

				FGameplayAbilitySpecHandle HandleToEnd = ActivePhase->Handle;
				GameState_ASC->CancelAbilitiesByFunc([HandleToEnd](const U<PROJECT_NAME>GameplayAbility* <PROJECT_NAME>Ability, FGameplayAbilitySpecHandle Handle) {
					return Handle == HandleToEnd;
				}, true);
			}
		}

		F<PROJECT_NAME>GamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(PhaseAbilityHandle);
		Entry.PhaseTag = IncomingPhaseTag;

		// Notify all observers of this phase that it has started.
		for (const FPhaseObserver& Observer : PhaseStartObservers)
		{
			if (Observer.IsMatch(IncomingPhaseTag))
			{
				Observer.PhaseCallback.ExecuteIfBound(IncomingPhaseTag);
			}
		}
	}
}

void U<PROJECT_NAME>GamePhaseSubsystem::OnEndPhase(const U<PROJECT_NAME>GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	const FGameplayTag EndedPhaseTag = PhaseAbility->GetGamePhaseTag();
	UE_LOG(Log<PROJECT_NAME>GamePhase, Log, TEXT("Ended Phase '%s' (%s)"), *EndedPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

	const F<PROJECT_NAME>GamePhaseEntry& Entry = ActivePhaseMap.FindChecked(PhaseAbilityHandle);
	Entry.PhaseEndedCallback.ExecuteIfBound(PhaseAbility);

	ActivePhaseMap.Remove(PhaseAbilityHandle);

	// Notify all observers of this phase that it has ended.
	for (const FPhaseObserver& Observer : PhaseEndObservers)
	{
		if (Observer.IsMatch(EndedPhaseTag))
		{
			Observer.PhaseCallback.ExecuteIfBound(EndedPhaseTag);
		}
	}
}

bool U<PROJECT_NAME>GamePhaseSubsystem::FPhaseObserver::IsMatch(const FGameplayTag& ComparePhaseTag) const
{
	switch(MatchType)
	{
	case EPhaseTagMatchType::ExactMatch:
		return ComparePhaseTag == PhaseTag;
	case EPhaseTagMatchType::PartialMatch:
		return ComparePhaseTag.MatchesTag(PhaseTag);
	}

	return false;
}