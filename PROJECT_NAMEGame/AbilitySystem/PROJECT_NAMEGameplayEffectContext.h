// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "<PROJECT_NAME>GameplayEffectContext.generated.h"

class UAbilitySystemComponent;
class I<PROJECT_NAME>AbilitySourceInterface;
class UPhysicalMaterial;

USTRUCT()
struct F<PROJECT_NAME>GameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	F<PROJECT_NAME>GameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	F<PROJECT_NAME>GameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped F<PROJECT_NAME>GameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static <PROJECT_NAME_UPPER>GAME_API F<PROJECT_NAME>GameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilitySource(const I<PROJECT_NAME>AbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const I<PROJECT_NAME>AbilitySourceInterface* GetAbilitySource() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		F<PROJECT_NAME>GameplayEffectContext* NewContext = new F<PROJECT_NAME>GameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return F<PROJECT_NAME>GameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;

public:
	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID = -1;

protected:
	/** Ability Source object (should implement I<PROJECT_NAME>AbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<F<PROJECT_NAME>GameplayEffectContext> : public TStructOpsTypeTraitsBase2<F<PROJECT_NAME>GameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

