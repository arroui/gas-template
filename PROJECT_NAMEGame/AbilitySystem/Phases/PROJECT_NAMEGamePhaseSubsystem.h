// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "<PROJECT_NAME>GamePhaseSubsystem.generated.h"

class U<PROJECT_NAME>GamePhaseAbility;

DECLARE_DYNAMIC_DELEGATE_OneParam(F<PROJECT_NAME>GamePhaseDynamicDelegate, const U<PROJECT_NAME>GamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(F<PROJECT_NAME>GamePhaseDelegate, const U<PROJECT_NAME>GamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(F<PROJECT_NAME>GamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(F<PROJECT_NAME>GamePhaseTagDelegate, const FGameplayTag& PhaseTag);

// Match rule for message receivers
UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
	// An exact match will only receive messages with exactly the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
	ExactMatch,

	// A partial match will receive any messages rooted in the same channel
	// (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
	PartialMatch
};


/**  */
UCLASS()
class U<PROJECT_NAME>GamePhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>GamePhaseSubsystem();

	virtual void PostInitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	void StartPhase(TSubclassOf<U<PROJECT_NAME>GamePhaseAbility> PhaseAbility, F<PROJECT_NAME>GamePhaseDelegate PhaseEndedCallback = F<PROJECT_NAME>GamePhaseDelegate());

	//TODO Return a handle so folks can delete these.  They will just grow until the world resets.
	//TODO Should we just occasionally clean these observers up?  It's not as if everyone will properly unhook them even if there is a handle.
	void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const F<PROJECT_NAME>GamePhaseTagDelegate& WhenPhaseActive);
	void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const F<PROJECT_NAME>GamePhaseTagDelegate& WhenPhaseEnd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
	bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName="Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<U<PROJECT_NAME>GamePhaseAbility> Phase, const F<PROJECT_NAME>GamePhaseDynamicDelegate& PhaseEnded);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
	void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, F<PROJECT_NAME>GamePhaseTagDynamicDelegate WhenPhaseActive);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
	void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, F<PROJECT_NAME>GamePhaseTagDynamicDelegate WhenPhaseEnd);

	void OnBeginPhase(const U<PROJECT_NAME>GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
	void OnEndPhase(const U<PROJECT_NAME>GamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
	struct F<PROJECT_NAME>GamePhaseEntry
	{
	public:
		FGameplayTag PhaseTag;
		F<PROJECT_NAME>GamePhaseDelegate PhaseEndedCallback;
	};

	TMap<FGameplayAbilitySpecHandle, F<PROJECT_NAME>GamePhaseEntry> ActivePhaseMap;

	struct FPhaseObserver
	{
	public:
		bool IsMatch(const FGameplayTag& ComparePhaseTag) const;
	
		FGameplayTag PhaseTag;
		EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
		F<PROJECT_NAME>GamePhaseTagDelegate PhaseCallback;
	};

	TArray<FPhaseObserver> PhaseStartObservers;
	TArray<FPhaseObserver> PhaseEndObservers;

	friend class U<PROJECT_NAME>GamePhaseAbility;
};
