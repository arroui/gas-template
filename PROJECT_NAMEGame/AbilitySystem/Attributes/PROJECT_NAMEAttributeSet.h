// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "<PROJECT_NAME>AttributeSet.generated.h"


class U<PROJECT_NAME>AbilitySystemComponent;
struct FGameplayEffectSpec;


/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(U<PROJECT_NAME>HealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(F<PROJECT_NAME>AttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);


/**
 * U<PROJECT_NAME>AttributeSet
 *
 *	Base attribute set class for the project.
 */
UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	U<PROJECT_NAME>AttributeSet();

	UWorld* GetWorld() const override;

	U<PROJECT_NAME>AbilitySystemComponent* Get<PROJECT_NAME>AbilitySystemComponent() const;
};
