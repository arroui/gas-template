// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesProjectPolicies.h"
#include "GameFeatureStateChangeObserver.h"

#include "<PROJECT_NAME>GameFeaturePolicy.generated.h"

class UGameFeatureData;

/**
 * Manager to keep track of the state machines that bring a game feature plugin into memory and active
 * This class discovers plugins either that are built-in and distributed with the game or are reported externally (i.e. by a web service or other endpoint)
 */
UCLASS(MinimalAPI, Config = Game)
class U<PROJECT_NAME>GameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()

public:
	<PROJECT_NAME_UPPER>GAME_API static U<PROJECT_NAME>GameFeaturePolicy& Get();

	U<PROJECT_NAME>GameFeaturePolicy(const FObjectInitializer& ObjectInitializer);

	//~UGameFeaturesProjectPolicies interface
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;
	virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(const UGameFeatureData* GameFeatureToLoad) const override;
	virtual bool IsPluginAllowed(const FString& PluginURL) const override;
	virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
	virtual void GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const override;
	//~End of UGameFeaturesProjectPolicies interface

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observers;
};



// checked
UCLASS()
class U<PROJECT_NAME>GameFeature_HotfixManager : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureLoading(const UGameFeatureData* GameFeatureData) override;
};

// checked
UCLASS()
class U<PROJECT_NAME>GameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName) override;
	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName) override;
};