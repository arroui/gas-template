// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>GameSettingRegistry.h"

#include "GameSetting.h"
#include "GameSettingValueDiscreteDynamic.h"
#include "GameSettingValueScalar.h"
#include "GameSettingValueScalarDynamic.h"
#include "GameSettingCollection.h"
#include "GameSettingAction.h"
#include "EditCondition/WhenPlayingAsPrimaryPlayer.h"
#include "CustomSettings/<PROJECT_NAME>SettingValueDiscrete_Language.h"
#include "Player/<PROJECT_NAME>LocalPlayer.h"

#define LOCTEXT_NAMESPACE "<PROJECT_NAME>"

UGameSettingCollection* U<PROJECT_NAME>GameSettingRegistry::InitializeGameplaySettings(U<PROJECT_NAME>LocalPlayer* InLocalPlayer)
{
	UGameSettingCollection* Screen = NewObject<UGameSettingCollection>();
	Screen->SetDevName(TEXT("GameplayCollection"));
	Screen->SetDisplayName(LOCTEXT("GameplayCollection_Name", "Gameplay"));
	Screen->Initialize(InLocalPlayer);

	{
		UGameSettingCollection* LanguageSubsection = NewObject<UGameSettingCollection>();
		LanguageSubsection->SetDevName(TEXT("LanguageCollection"));
		LanguageSubsection->SetDisplayName(LOCTEXT("LanguageCollection_Name", "Language"));
		Screen->AddSetting(LanguageSubsection);

		//----------------------------------------------------------------------------------
		{
			U<PROJECT_NAME>SettingValueDiscrete_Language* Setting = NewObject<U<PROJECT_NAME>SettingValueDiscrete_Language>();
			Setting->SetDevName(TEXT("Language"));
			Setting->SetDisplayName(LOCTEXT("LanguageSetting_Name", "Language"));
			Setting->SetDescriptionRichText(LOCTEXT("LanguageSetting_Description", "The language of the game."));
			
#if WITH_EDITOR
			if (GIsEditor)
			{
				Setting->SetDescriptionRichText(LOCTEXT("LanguageSetting_WithEditor_Description", "The language of the game.\n\n<text color=\"#ffff00\">WARNING: Language changes will not affect PIE, you'll need to run with -game to test this, or change your PIE language options in the editor preferences.</>"));
			}
#endif
			
			Setting->AddEditCondition(FWhenPlayingAsPrimaryPlayer::Get());

			LanguageSubsection->AddSetting(Setting);
		}
		//----------------------------------------------------------------------------------
	}

	return Screen;
}

#undef LOCTEXT_NAMESPACE
