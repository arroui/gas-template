// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>LoadingScreenSubsystem.h"

//////////////////////////////////////////////////////////////////////
// U<PROJECT_NAME>LoadingScreenSubsystem

U<PROJECT_NAME>LoadingScreenSubsystem::U<PROJECT_NAME>LoadingScreenSubsystem()
{
}

void U<PROJECT_NAME>LoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> U<PROJECT_NAME>LoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}
