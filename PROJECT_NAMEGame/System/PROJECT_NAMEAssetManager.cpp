// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AssetManager.h"
#include "<PROJECT_NAME>LogChannels.h"
#include "<PROJECT_NAME>GameplayTags.h"
#include "<PROJECT_NAME>GameData.h"
#include "AbilitySystemGlobals.h"
#include "Character/<PROJECT_NAME>PawnData.h"
#include "Stats/StatsMisc.h"
#include "Engine/Engine.h"
#include "AbilitySystem/<PROJECT_NAME>GameplayCueManager.h"
#include "Misc/ScopedSlowTask.h"

const FName F<PROJECT_NAME>Bundles::Equipped("Equipped");

//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("<PROJECT_NAME>.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(U<PROJECT_NAME>AssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(F<PROJECT_NAME>AssetManagerStartupJob(#JobFunc, [this](const F<PROJECT_NAME>AssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////

U<PROJECT_NAME>AssetManager::U<PROJECT_NAME>AssetManager()
{
	DefaultPawnData = nullptr;
}

U<PROJECT_NAME>AssetManager& U<PROJECT_NAME>AssetManager::Get()
{
	check(GEngine);

	if (U<PROJECT_NAME>AssetManager* Singleton = Cast<U<PROJECT_NAME>AssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(Log<PROJECT_NAME>, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to <PROJECT_NAME>AssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<U<PROJECT_NAME>AssetManager>();
}

UObject* U<PROJECT_NAME>AssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool U<PROJECT_NAME>AssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void U<PROJECT_NAME>AssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void U<PROJECT_NAME>AssetManager::DumpLoadedAssets()
{
	UE_LOG(Log<PROJECT_NAME>, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(Log<PROJECT_NAME>, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(Log<PROJECT_NAME>, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(Log<PROJECT_NAME>, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

void U<PROJECT_NAME>AssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("U<PROJECT_NAME>AssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeAbilitySystem());
	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// Load base game data asset
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	// Run all the queued up startup jobs
	DoAllStartupJobs();
}

void U<PROJECT_NAME>AssetManager::InitializeAbilitySystem()
{
	SCOPED_BOOT_TIMING("U<PROJECT_NAME>AssetManager::InitializeAbilitySystem");

	F<PROJECT_NAME>GameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

void U<PROJECT_NAME>AssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("U<PROJECT_NAME>AssetManager::InitializeGameplayCueManager");

	U<PROJECT_NAME>GameplayCueManager* GCM = U<PROJECT_NAME>GameplayCueManager::Get();
	check(GCM);
	GCM->LoadAlwaysLoadedCues();
}


const U<PROJECT_NAME>GameData& U<PROJECT_NAME>AssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<U<PROJECT_NAME>GameData>(<PROJECT_NAME>GameDataPath);
}

const U<PROJECT_NAME>PawnData* U<PROJECT_NAME>AssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UPrimaryDataAsset* U<PROJECT_NAME>AssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("<PROJECT_NAME>Editor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(Log<PROJECT_NAME>, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(Log<PROJECT_NAME>, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}


void U<PROJECT_NAME>AssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("U<PROJECT_NAME>AssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const F<PROJECT_NAME>AssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.0f;
			for (const F<PROJECT_NAME>AssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.0f;
			for (F<PROJECT_NAME>AssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(Log<PROJECT_NAME>, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void U<PROJECT_NAME>AssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}

#if WITH_EDITOR
void U<PROJECT_NAME>AssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("<PROJECT_NAME>Editor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const U<PROJECT_NAME>GameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif