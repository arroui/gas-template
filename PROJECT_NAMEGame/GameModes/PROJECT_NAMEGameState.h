// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "AbilitySystemInterface.h"
#include "Messages/<PROJECT_NAME>VerbMessage.h"

#include "<PROJECT_NAME>GameState.generated.h"

class U<PROJECT_NAME>ExperienceManagerComponent;
class U<PROJECT_NAME>AbilitySystemComponent;
class UAbilitySystemComponent;

/**
 * A<PROJECT_NAME>GameState
 *
 *	The base game state class used by this project.
 */
UCLASS(Config = Game)
class <PROJECT_NAME_UPPER>GAME_API A<PROJECT_NAME>GameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	A<PROJECT_NAME>GameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetServerFPS() const { return ServerFPS; }

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category = "<PROJECT_NAME>|GameState")
	U<PROJECT_NAME>AbilitySystemComponent* Get<PROJECT_NAME>AbilitySystemComponent() const { return AbilitySystemComponent; }

	// Send a message that all clients will (probably) get
	// (use only for client notifications like eliminations, server join messages, etc... that can handle being lost)
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "<PROJECT_NAME>|GameState")
	void MulticastMessageToClients(const F<PROJECT_NAME>VerbMessage Message);

	// Send a message that all clients will be guaranteed to get
	// (use only for client notifications that cannot handle being lost)
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "<PROJECT_NAME>|GameState")
	void MulticastReliableMessageToClients(const F<PROJECT_NAME>VerbMessage Message);

private:
	UPROPERTY()
	U<PROJECT_NAME>ExperienceManagerComponent* ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "<PROJECT_NAME>|GameState")
	U<PROJECT_NAME>AbilitySystemComponent* AbilitySystemComponent;


protected:

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(Replicated)
	float ServerFPS;
};
