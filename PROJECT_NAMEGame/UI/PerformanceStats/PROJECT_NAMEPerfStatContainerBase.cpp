// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>PerfStatContainerBase.h"
#include "Settings/<PROJECT_NAME>SettingsLocal.h"
#include "<PROJECT_NAME>PerfStatWidgetBase.h"
#include "Blueprint/WidgetTree.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>PerfStatsContainerBase

U<PROJECT_NAME>PerfStatContainerBase::U<PROJECT_NAME>PerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void U<PROJECT_NAME>PerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	U<PROJECT_NAME>SettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void U<PROJECT_NAME>PerfStatContainerBase::NativeDestruct()
{
	U<PROJECT_NAME>SettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void U<PROJECT_NAME>PerfStatContainerBase::UpdateVisibilityOfChildren()
{
	U<PROJECT_NAME>SettingsLocal* UserSettings = U<PROJECT_NAME>SettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == E<PROJECT_NAME>StatDisplayMode::TextOnly) || (StatDisplayModeFilter == E<PROJECT_NAME>StatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == E<PROJECT_NAME>StatDisplayMode::GraphOnly) || (StatDisplayModeFilter == E<PROJECT_NAME>StatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (U<PROJECT_NAME>PerfStatWidgetBase* TypedWidget = Cast<U<PROJECT_NAME>PerfStatWidgetBase>(Widget))
		{
			const E<PROJECT_NAME>StatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case E<PROJECT_NAME>StatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case E<PROJECT_NAME>StatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case E<PROJECT_NAME>StatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case E<PROJECT_NAME>StatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}
