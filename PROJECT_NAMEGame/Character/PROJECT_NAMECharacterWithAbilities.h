// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/<PROJECT_NAME>Character.h"
#include "<PROJECT_NAME>CharacterWithAbilities.generated.h"

// A<PROJECT_NAME>Character typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(Blueprintable)
class A<PROJECT_NAME>CharacterWithAbilities : public A<PROJECT_NAME>Character
{
	GENERATED_BODY()

public:
	A<PROJECT_NAME>CharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "<PROJECT_NAME>|PlayerState")
	U<PROJECT_NAME>AbilitySystemComponent* AbilitySystemComponent;
};
