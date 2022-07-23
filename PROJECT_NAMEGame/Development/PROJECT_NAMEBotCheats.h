// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"

#include "<PROJECT_NAME>BotCheats.generated.h"

class U<PROJECT_NAME>BotCreationComponent;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class U<PROJECT_NAME>BotCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>BotCheats();

	// Adds a bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddPlayerBot();

	// Removes a random bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void RemovePlayerBot();

private:
	U<PROJECT_NAME>BotCreationComponent* GetBotComponent() const;
};
