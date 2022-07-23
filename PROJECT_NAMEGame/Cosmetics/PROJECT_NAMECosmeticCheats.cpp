// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>CosmeticCheats.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "<PROJECT_NAME>ControllerComponent_CharacterParts.h"
#include "System/<PROJECT_NAME>DevelopmentStatics.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>CosmeticCheats

U<PROJECT_NAME>CosmeticCheats::U<PROJECT_NAME>CosmeticCheats()
{
#if UE_WITH_CHEAT_MANAGER
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

void U<PROJECT_NAME>CosmeticCheats::AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	if (U<PROJECT_NAME>ControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		TSubclassOf<AActor> PartClass = U<PROJECT_NAME>DevelopmentStatics::FindClassByShortName<AActor>(AssetName);
		if (PartClass != nullptr)
		{
			F<PROJECT_NAME>CharacterPart Part;
			Part.PartClass = PartClass;

			CosmeticComponent->AddCheatPart(Part, bSuppressNaturalParts);
		}
	}
#endif	
}

void U<PROJECT_NAME>CosmeticCheats::ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
	ClearCharacterPartOverrides();
	AddCharacterPart(AssetName, bSuppressNaturalParts);
}

void U<PROJECT_NAME>CosmeticCheats::ClearCharacterPartOverrides()
{
#if UE_WITH_CHEAT_MANAGER
	if (U<PROJECT_NAME>ControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		CosmeticComponent->ClearCheatParts();
	}
#endif	
}

U<PROJECT_NAME>ControllerComponent_CharacterParts* U<PROJECT_NAME>CosmeticCheats::GetCosmeticComponent() const
{
	if (APlayerController* PC = GetPlayerController())
	{
		return PC->FindComponentByClass<U<PROJECT_NAME>ControllerComponent_CharacterParts>();
	}

	return nullptr;
}
