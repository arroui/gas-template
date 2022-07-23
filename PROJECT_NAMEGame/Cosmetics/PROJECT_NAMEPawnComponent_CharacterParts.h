// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Components/PawnComponent.h"
#include "<PROJECT_NAME>CharacterPartTypes.h"
#include "GameplayTagContainer.h"
#include "Cosmetics/<PROJECT_NAME>CosmeticAnimationTypes.h"

#include "<PROJECT_NAME>PawnComponent_CharacterParts.generated.h"

class USkeletalMeshComponent;
class UChildActorComponent;
class U<PROJECT_NAME>PawnComponent_CharacterParts;
struct F<PROJECT_NAME>CharacterPartList;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F<PROJECT_NAME>SpawnedCharacterPartsChanged, U<PROJECT_NAME>PawnComponent_CharacterParts*, ComponentWithChangedParts);

//////////////////////////////////////////////////////////////////////

// A single applied character part
USTRUCT()
struct F<PROJECT_NAME>AppliedCharacterPartEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	F<PROJECT_NAME>AppliedCharacterPartEntry()
	{}

	FString GetDebugString() const;

private:
	friend F<PROJECT_NAME>CharacterPartList;
	friend U<PROJECT_NAME>PawnComponent_CharacterParts;

private:
	// The character part being represented
	UPROPERTY()
	F<PROJECT_NAME>CharacterPart Part;

	// Handle index we returned to the user (server only)
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// The spawned actor instance (client only)
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////

// Replicated list of applied character parts
USTRUCT(BlueprintType)
struct F<PROJECT_NAME>CharacterPartList : public FFastArraySerializer
{
	GENERATED_BODY()

	F<PROJECT_NAME>CharacterPartList()
		: OwnerComponent(nullptr)
	{
	}

	F<PROJECT_NAME>CharacterPartList(U<PROJECT_NAME>PawnComponent_CharacterParts* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<F<PROJECT_NAME>AppliedCharacterPartEntry, F<PROJECT_NAME>CharacterPartList>(Entries, DeltaParms, *this);
	}

	F<PROJECT_NAME>CharacterPartHandle AddEntry(F<PROJECT_NAME>CharacterPart NewPart);
	void RemoveEntry(F<PROJECT_NAME>CharacterPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;
private:
	friend U<PROJECT_NAME>PawnComponent_CharacterParts;

	bool SpawnActorForEntry(F<PROJECT_NAME>AppliedCharacterPartEntry& Entry);
	bool DestroyActorForEntry(F<PROJECT_NAME>AppliedCharacterPartEntry& Entry);

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<F<PROJECT_NAME>AppliedCharacterPartEntry> Entries;

	// The component that contains this list
	UPROPERTY()
	U<PROJECT_NAME>PawnComponent_CharacterParts* OwnerComponent;

	// Upcounter for handles
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<F<PROJECT_NAME>CharacterPartList> : public TStructOpsTypeTraitsBase2<F<PROJECT_NAME>CharacterPartList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

// A component that handles spawning cosmetic actors attached to the owner pawn on all clients
UCLASS(meta=(BlueprintSpawnableComponent))
class U<PROJECT_NAME>PawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>PawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	F<PROJECT_NAME>CharacterPartHandle AddCharacterPart(const F<PROJECT_NAME>CharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(F<PROJECT_NAME>CharacterPartHandle Handle);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Gets the list of all spawned character parts from this component
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// If the parent actor is derived from ACharacter, returns the Mesh component, otherwise nullptr
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// Returns the scene component to attach the spawned actors to
	// If the parent actor is derived from ACharacter, we'll use the Mesh component, otherwise the root component
	USceneComponent* GetSceneComponentToAttachTo() const;

	// Returns the set of combined gameplay tags from attached character parts, optionally filtered to only tags that start with the specified root
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// Delegate that will be called when the list of spawned character parts has changed
	UPROPERTY(BlueprintAssignable, Category=Cosmetics, BlueprintCallable)
	F<PROJECT_NAME>SpawnedCharacterPartsChanged OnCharacterPartsChanged;

private:
	// List of character parts
	UPROPERTY(Replicated)
	F<PROJECT_NAME>CharacterPartList CharacterPartList;

	// Rules for how to pick a body style mesh for animation to play on, based on character part cosmetics tags
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	F<PROJECT_NAME>AnimBodyStyleSelectionSet BodyMeshes;
};
