// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"
#include "<PROJECT_NAME>WidgetFactory.h"

#include "<PROJECT_NAME>WidgetFactory_Class.generated.h"

UCLASS()
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>WidgetFactory_Class : public U<PROJECT_NAME>WidgetFactory
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>WidgetFactory_Class() { }

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};