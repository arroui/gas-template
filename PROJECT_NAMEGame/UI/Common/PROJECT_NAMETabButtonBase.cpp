// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TabButtonBase.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"

void U<PROJECT_NAME>TabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void U<PROJECT_NAME>TabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void U<PROJECT_NAME>TabButtonBase::SetTabLabelInfo_Implementation(const F<PROJECT_NAME>TabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}
