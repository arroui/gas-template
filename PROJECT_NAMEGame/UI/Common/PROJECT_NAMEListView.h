// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonListView.h"
#include "Components/ListView.h"

#include "<PROJECT_NAME>ListView.generated.h"

class UUserWidget;
class ULocalPlayer;
class U<PROJECT_NAME>WidgetFactory;

UCLASS(meta = (DisableNativeTick))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>ListView : public UCommonListView
{
	GENERATED_BODY()

public:
	U<PROJECT_NAME>ListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	//virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category="Entry Creation")
	TArray<TObjectPtr<U<PROJECT_NAME>WidgetFactory>> FactoryRules;
};