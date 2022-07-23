// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>BotCheats.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/<PROJECT_NAME>BotCreationComponent.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>BotCheats

U<PROJECT_NAME>BotCheats::U<PROJECT_NAME>BotCheats()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void U<PROJECT_NAME>BotCheats::AddPlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (U<PROJECT_NAME>BotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_AddBot();
	}
#endif	
}

void U<PROJECT_NAME>BotCheats::RemovePlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (U<PROJECT_NAME>BotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_RemoveBot();
	}
#endif	
}

U<PROJECT_NAME>BotCreationComponent* U<PROJECT_NAME>BotCheats::GetBotComponent() const
{
	if (UWorld* World = GetWorld())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->FindComponentByClass<U<PROJECT_NAME>BotCreationComponent>();
		}
	}

	return nullptr;
}
