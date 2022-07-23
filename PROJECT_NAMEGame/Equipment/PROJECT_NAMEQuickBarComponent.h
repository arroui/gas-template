// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "<PROJECT_NAME>QuickBarComponent.generated.h"

class U<PROJECT_NAME>InventoryItemInstance;
class U<PROJECT_NAME>EquipmentInstance;
class U<PROJECT_NAME>EquipmentManagerComponent;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class U<PROJECT_NAME>QuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>QuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="<PROJECT_NAME>")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="<PROJECT_NAME>")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="<PROJECT_NAME>")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<U<PROJECT_NAME>InventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	U<PROJECT_NAME>InventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, U<PROJECT_NAME>InventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	U<PROJECT_NAME>InventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	U<PROJECT_NAME>EquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<U<PROJECT_NAME>InventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<U<PROJECT_NAME>EquipmentInstance> EquippedItem;
};


USTRUCT(BlueprintType)
struct F<PROJECT_NAME>QuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	AActor* Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<U<PROJECT_NAME>InventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct F<PROJECT_NAME>QuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	AActor* Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
