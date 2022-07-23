// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"

#include "<PROJECT_NAME>WidgetFactory.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>WidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>WidgetFactory() { }

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};