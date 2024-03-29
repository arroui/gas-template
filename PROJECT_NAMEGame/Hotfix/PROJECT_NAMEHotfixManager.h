// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectKey.h"
#include "OnlineHotfixManager.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Tickable.h"
#include "Misc/CoreDelegates.h"
#include "Containers/Ticker.h"
#include "<PROJECT_NAME>HotfixManager.generated.h"

UCLASS()
class U<PROJECT_NAME>HotfixManager : public UOnlineHotfixManager
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPendingGameHotfix, bool);
	FOnPendingGameHotfix OnPendingGameHotfixChanged;

	U<PROJECT_NAME>HotfixManager();
	virtual ~U<PROJECT_NAME>HotfixManager();

	void RequestPatchAssetsFromIniFiles();

protected:
	void OnHotfixCompleted(EHotfixResult HotfixResult);

	virtual FString GetCachedDirectory() override
	{
		return FPaths::ProjectPersistentDownloadDir() / TEXT("Hotfix/");
	}

	virtual void StartHotfixProcess() override;

	virtual bool WantsHotfixProcessing(const struct FCloudFileHeader& FileHeader) override;
	virtual bool ApplyHotfixProcessing(const struct FCloudFileHeader& FileHeader) override;
	virtual bool ShouldWarnAboutMissingWhenPatchingFromIni(const FString& AssetPath) const override;
	virtual void PatchAssetsFromIniFiles() override;

	virtual bool PreProcessDownloadedFileData(TArray<uint8>& FileData) const override;
	virtual void OnHotfixAvailablityCheck(const TArray<FCloudFileHeader>& PendingChangedFiles, const TArray<FCloudFileHeader>& PendingRemoveFiles) override;

	virtual bool HotfixIniFile(const FString& FileName, const FString& IniData) override;

private:
#if !UE_BUILD_SHIPPING
	// Error reporting
	FDelegateHandle OnScreenMessageHandle;
	void GetOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages);
#endif // !UE_BUILD_SHIPPING

private:
	/** get the current game instance */
	template<typename T>
	T* GetGameInstance() const
	{
		return GetTypedOuter<T>();
	}

	void Init() override;

private:
	FTSTicker::FDelegateHandle RequestPatchAssetsHandle;
	FDelegateHandle HotfixCompleteDelegateHandle;

	bool bHasPendingGameHotfix = false;
	bool bHasPendingDeviceProfileHotfix = false;

	static int32 GameHotfixCounter;
};
