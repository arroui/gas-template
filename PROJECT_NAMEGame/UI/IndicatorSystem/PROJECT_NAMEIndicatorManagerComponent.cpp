// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>IndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"

U<PROJECT_NAME>IndicatorManagerComponent::U<PROJECT_NAME>IndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

/*static*/ U<PROJECT_NAME>IndicatorManagerComponent* U<PROJECT_NAME>IndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<U<PROJECT_NAME>IndicatorManagerComponent>();
	}

	return nullptr;
}

void U<PROJECT_NAME>IndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void U<PROJECT_NAME>IndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);
	
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}