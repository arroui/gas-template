// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>WeaponUserInterface.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/<PROJECT_NAME>WeaponInstance.h"
#include "Equipment/<PROJECT_NAME>EquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"

U<PROJECT_NAME>WeaponUserInterface::U<PROJECT_NAME>WeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>WeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void U<PROJECT_NAME>WeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void U<PROJECT_NAME>WeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (U<PROJECT_NAME>EquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<U<PROJECT_NAME>EquipmentManagerComponent>())
		{
			if (U<PROJECT_NAME>WeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<U<PROJECT_NAME>WeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					U<PROJECT_NAME>WeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void U<PROJECT_NAME>WeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}
