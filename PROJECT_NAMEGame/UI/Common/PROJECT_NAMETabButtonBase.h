// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonButtonBase.h"
#include "<PROJECT_NAME>TabListWidgetBase.h"
#include "UI/Foundation/<PROJECT_NAME>ButtonBase.h"

#include "<PROJECT_NAME>TabButtonBase.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>TabButtonBase : public U<PROJECT_NAME>ButtonBase, public I<PROJECT_NAME>TabButtonInterface
{
	GENERATED_BODY()

public:

	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const F<PROJECT_NAME>TabDescriptor& TabLabelInfo) override;

private:

	UPROPERTY(meta = (BindWidgetOptional))
	UCommonLazyImage* LazyImage_Icon;
};
