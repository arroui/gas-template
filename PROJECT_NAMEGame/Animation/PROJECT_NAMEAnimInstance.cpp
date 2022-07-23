// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/<PROJECT_NAME>Character.h"
#include "Character/<PROJECT_NAME>CharacterMovementComponent.h"


U<PROJECT_NAME>AnimInstance::U<PROJECT_NAME>AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>AnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult U<PROJECT_NAME>AnimInstance::IsDataValid(TArray<FText>& ValidationErrors)
{
	Super::IsDataValid(ValidationErrors);

	GameplayTagPropertyMap.IsDataValid(this, ValidationErrors);

	return ((ValidationErrors.Num() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void U<PROJECT_NAME>AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void U<PROJECT_NAME>AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const A<PROJECT_NAME>Character* Character = Cast<A<PROJECT_NAME>Character>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	U<PROJECT_NAME>CharacterMovementComponent* CharMoveComp = CastChecked<U<PROJECT_NAME>CharacterMovementComponent>(Character->GetCharacterMovement());
	const F<PROJECT_NAME>CharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}
