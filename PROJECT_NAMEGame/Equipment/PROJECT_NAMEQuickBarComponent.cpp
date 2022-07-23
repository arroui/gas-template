// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>QuickBarComponent.h"

#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "Net/UnrealNetwork.h"
#include "Inventory/<PROJECT_NAME>InventoryItemInstance.h"
#include "Inventory/<PROJECT_NAME>InventoryItemDefinition.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "Equipment/<PROJECT_NAME>EquipmentInstance.h"
#include "Equipment/<PROJECT_NAME>EquipmentDefinition.h"
#include "Equipment/<PROJECT_NAME>EquipmentManagerComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_<PROJECT_NAME>_QuickBar_Message_SlotsChanged, "<PROJECT_NAME>.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_<PROJECT_NAME>_QuickBar_Message_ActiveIndexChanged, "<PROJECT_NAME>.QuickBar.Message.ActiveIndexChanged");

U<PROJECT_NAME>QuickBarComponent::U<PROJECT_NAME>QuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void U<PROJECT_NAME>QuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void U<PROJECT_NAME>QuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void U<PROJECT_NAME>QuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void U<PROJECT_NAME>QuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void U<PROJECT_NAME>QuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (U<PROJECT_NAME>InventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<U<PROJECT_NAME>EquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (U<PROJECT_NAME>EquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (EquippedItem != nullptr)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

void U<PROJECT_NAME>QuickBarComponent::UnequipItemInSlot()
{
	if (U<PROJECT_NAME>EquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

U<PROJECT_NAME>EquipmentManagerComponent* U<PROJECT_NAME>QuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<U<PROJECT_NAME>EquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void U<PROJECT_NAME>QuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_ActiveSlotIndex();
	}
}

U<PROJECT_NAME>InventoryItemInstance* U<PROJECT_NAME>QuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 U<PROJECT_NAME>QuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (TObjectPtr<U<PROJECT_NAME>InventoryItemInstance> ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void U<PROJECT_NAME>QuickBarComponent::AddItemToSlot(int32 SlotIndex, U<PROJECT_NAME>InventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}
}

U<PROJECT_NAME>InventoryItemInstance* U<PROJECT_NAME>QuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	U<PROJECT_NAME>InventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void U<PROJECT_NAME>QuickBarComponent::OnRep_Slots()
{
	F<PROJECT_NAME>QuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_<PROJECT_NAME>_QuickBar_Message_SlotsChanged, Message);
}

void U<PROJECT_NAME>QuickBarComponent::OnRep_ActiveSlotIndex()
{
	F<PROJECT_NAME>QuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_<PROJECT_NAME>_QuickBar_Message_ActiveIndexChanged, Message);
}
