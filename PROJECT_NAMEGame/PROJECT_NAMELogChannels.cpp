// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>LogChannels.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>);
DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>Experience);
DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>AbilitySystem);
DEFINE_LOG_CATEGORY(Log<PROJECT_NAME>Teams);

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}
