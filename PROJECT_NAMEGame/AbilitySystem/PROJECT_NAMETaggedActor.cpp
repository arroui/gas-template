// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TaggedActor.h"

A<PROJECT_NAME>TaggedActor::A<PROJECT_NAME>TaggedActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void A<PROJECT_NAME>TaggedActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(StaticGameplayTags);
}

#if WITH_EDITOR
bool A<PROJECT_NAME>TaggedActor::CanEditChange(const FProperty* InProperty) const
{
	// Prevent editing of the other tags property
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AActor, Tags))
	{
		return false;
	}

	return Super::CanEditChange(InProperty);
}
#endif
