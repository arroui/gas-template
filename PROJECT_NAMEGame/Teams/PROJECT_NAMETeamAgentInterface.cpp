// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>TeamAgentInterface.h"
#include "<PROJECT_NAME>LogChannels.h"

U<PROJECT_NAME>TeamAgentInterface::U<PROJECT_NAME>TeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void I<PROJECT_NAME>TeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<I<PROJECT_NAME>TeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(Log<PROJECT_NAME>Teams, Verbose, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}
