// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "System/GameplayTagStack.h"
#include "Messages/<PROJECT_NAME>VerbMessage.h"
#include "Teams/<PROJECT_NAME>TeamAgentInterface.h"

#include "<PROJECT_NAME>PlayerState.generated.h"


class A<PROJECT_NAME>PlayerController;
class U<PROJECT_NAME>AbilitySystemComponent;
class UAbilitySystemComponent;
class U<PROJECT_NAME>PawnData;
class U<PROJECT_NAME>ExperienceDefinition;

/** Defines the types of client connected */
UENUM()
enum class E<PROJECT_NAME>PlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * A<PROJECT_NAME>PlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class <PROJECT_NAME_UPPER>GAME_API A<PROJECT_NAME>PlayerState : public AModularPlayerState, public IAbilitySystemInterface, public I<PROJECT_NAME>TeamAgentInterface
{
	GENERATED_BODY()

public:
	A<PROJECT_NAME>PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|PlayerState")
	A<PROJECT_NAME>PlayerController* Get<PROJECT_NAME>PlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|PlayerState")
	U<PROJECT_NAME>AbilitySystemComponent* Get<PROJECT_NAME>AbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const U<PROJECT_NAME>PawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	//~I<PROJECT_NAME>TeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOn<PROJECT_NAME>TeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of I<PROJECT_NAME>TeamAgentInterface interface

	static const FName NAME_<PROJECT_NAME>AbilityReady;

	void SetPlayerConnectionType(E<PROJECT_NAME>PlayerConnectionType NewType);
	E<PROJECT_NAME>PlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	/** Returns the Squad ID of the squad the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	void SetSquadID(int32 NewSquadID);

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "<PROJECT_NAME>|PlayerState")
	void ClientBroadcastMessage(const F<PROJECT_NAME>VerbMessage Message);

private:
	void OnExperienceLoaded(const U<PROJECT_NAME>ExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	const U<PROJECT_NAME>PawnData* PawnData;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "<PROJECT_NAME>|PlayerState")
	U<PROJECT_NAME>AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Replicated)
	E<PROJECT_NAME>PlayerConnectionType MyPlayerConnectionType;

	UPROPERTY()
	FOn<PROJECT_NAME>TeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
	int32 MySquadID;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnRep_MySquadID();
};
