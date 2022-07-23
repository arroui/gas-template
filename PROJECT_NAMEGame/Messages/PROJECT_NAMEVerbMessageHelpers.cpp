// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>VerbMessageHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

//////////////////////////////////////////////////////////////////////
// F<PROJECT_NAME>VerbMessage

FString F<PROJECT_NAME>VerbMessage::ToString() const
{
	FString HumanReadableMessage;
	F<PROJECT_NAME>VerbMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage, this, /*Defaults=*/ nullptr, /*OwnerObject=*/ nullptr, PPF_None, /*ExportRootScope=*/ nullptr);
	return HumanReadableMessage;
}

//////////////////////////////////////////////////////////////////////
// 

APlayerState* U<PROJECT_NAME>VerbMessageHelpers::GetPlayerStateFromObject(UObject* Object)
{
	if (APlayerController* PC = Cast<APlayerController>(Object))
	{
		return PC->PlayerState;
	}

	if (APlayerState* TargetPS = Cast<APlayerState>(Object))
	{
		return TargetPS;
	}
	
	if (APawn* TargetPawn = Cast<APawn>(Object))
	{
		if (APlayerState* TargetPS = TargetPawn->GetPlayerState())
		{
			return TargetPS;
		}
	}
	return nullptr;
}

APlayerController* U<PROJECT_NAME>VerbMessageHelpers::GetPlayerControllerFromObject(UObject* Object)
{
	if (APlayerController* PC = Cast<APlayerController>(Object))
	{
		return PC;
	}

	if (APlayerState* TargetPS = Cast<APlayerState>(Object))
	{
		return TargetPS->GetPlayerController();
	}

	if (APawn* TargetPawn = Cast<APawn>(Object))
	{
		return Cast<APlayerController>(TargetPawn->GetController());
	}

	return nullptr;
}

FGameplayCueParameters U<PROJECT_NAME>VerbMessageHelpers::VerbMessageToCueParameters(const F<PROJECT_NAME>VerbMessage& Message)
{
	FGameplayCueParameters Result;

	Result.OriginalTag = Message.Verb;
	Result.Instigator = Cast<AActor>(Message.Instigator);
	Result.EffectCauser = Cast<AActor>(Message.Target);
	Result.AggregatedSourceTags = Message.InstigatorTags;
	Result.AggregatedTargetTags = Message.TargetTags;
	//@TODO: = Message.ContextTags;
	Result.RawMagnitude = Message.Magnitude;

	return Result;
}

F<PROJECT_NAME>VerbMessage U<PROJECT_NAME>VerbMessageHelpers::CueParametersToVerbMessage(const FGameplayCueParameters& Params)
{
	F<PROJECT_NAME>VerbMessage Result;
	
	Result.Verb = Params.OriginalTag;
	Result.Instigator = Params.Instigator.Get();
	Result.Target = Params.EffectCauser.Get();
	Result.InstigatorTags = Params.AggregatedSourceTags;
	Result.TargetTags = Params.AggregatedTargetTags;
	//@TODO: Result.ContextTags = ???;
	Result.Magnitude = Params.RawMagnitude;

	return Result;
}
