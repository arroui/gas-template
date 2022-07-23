// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ContextEffectsLibraryFactory.h"
#include "Feedback/ContextEffects/<PROJECT_NAME>ContextEffectsLibrary.h"

U<PROJECT_NAME>ContextEffectsLibraryFactory::U<PROJECT_NAME>ContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = U<PROJECT_NAME>ContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* U<PROJECT_NAME>ContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	U<PROJECT_NAME>ContextEffectsLibrary* <PROJECT_NAME>ContextEffectsLibrary = NewObject<U<PROJECT_NAME>ContextEffectsLibrary>(InParent, Name, Flags);

	return <PROJECT_NAME>ContextEffectsLibrary;
}