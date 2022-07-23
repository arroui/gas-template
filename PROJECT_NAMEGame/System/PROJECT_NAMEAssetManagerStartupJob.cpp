// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>AssetManagerStartupJob.h"
#include "<PROJECT_NAME>LogChannels.h"

TSharedPtr<FStreamableHandle> F<PROJECT_NAME>AssetManagerStartupJob::DoJob() const
{
	const double JobStartTime = FPlatformTime::Seconds();

	TSharedPtr<FStreamableHandle> Handle;
	UE_LOG(Log<PROJECT_NAME>, Display, TEXT("Startup job \"%s\" starting"), *JobName);
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &F<PROJECT_NAME>AssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	UE_LOG(Log<PROJECT_NAME>, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
