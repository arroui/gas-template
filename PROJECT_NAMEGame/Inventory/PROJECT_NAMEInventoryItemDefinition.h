// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "<PROJECT_NAME>InventoryItemDefinition.generated.h"

class U<PROJECT_NAME>InventoryItemInstance;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class U<PROJECT_NAME>InventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(U<PROJECT_NAME>InventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * U<PROJECT_NAME>InventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class U<PROJECT_NAME>InventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>InventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<U<PROJECT_NAME>InventoryItemFragment>> Fragments;

public:
	const U<PROJECT_NAME>InventoryItemFragment* FindFragmentByClass(TSubclassOf<U<PROJECT_NAME>InventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class U<PROJECT_NAME>InventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const U<PROJECT_NAME>InventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<U<PROJECT_NAME>InventoryItemDefinition> ItemDef, TSubclassOf<U<PROJECT_NAME>InventoryItemFragment> FragmentClass);
};
