// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"

class FAssetTypeActions_<PROJECT_NAME>ContextEffectsLibrary : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_<PROJECT_NAME>ContextEffectsLibrary", "<PROJECT_NAME>ContextEffectsLibrary"); }
	virtual FColor GetTypeColor() const override { return FColor(65, 200, 98); }
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
};
