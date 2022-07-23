// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonTabListWidgetBase.h"
#include "CommonUserWidget.h"
#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"

#include "<PROJECT_NAME>TabListWidgetBase.generated.h"

class UCommonButtonBase;

USTRUCT(BlueprintType)
struct F<PROJECT_NAME>TabDescriptor
{
	GENERATED_BODY()

public:
	F<PROJECT_NAME>TabDescriptor()
	: bHidden(false)
	, CreatedTabContentWidget(nullptr)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHidden;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButtonType;

	//TODO NDarnell - This should become a TSoftClassPtr<>, the underlying common tab list needs to be able to handle lazy tab content construction.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonUserWidget> TabContentType;

	UPROPERTY(Transient)
	UWidget* CreatedTabContentWidget;
};

UINTERFACE(BlueprintType)
class U<PROJECT_NAME>TabButtonInterface : public UInterface
{
	GENERATED_BODY()
};

class I<PROJECT_NAME>TabButtonInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Tab Button")
	void SetTabLabelInfo(const F<PROJECT_NAME>TabDescriptor& TabDescriptor);
};

UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
class <PROJECT_NAME_UPPER>GAME_API U<PROJECT_NAME>TabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool GetPreregisteredTabInfo(const FName TabNameId, F<PROJECT_NAME>TabDescriptor& OutTabInfo);

	/** Helper method to get at all the preregistered tab infos */
	const TArray<F<PROJECT_NAME>TabDescriptor>& GetAllPreregisteredTabInfos() { return PreregisteredTabInfoArray; }

	// Toggles whether or not a specified tab is hidden, can only be called before the switcher is associated
	UFUNCTION(BlueprintCallable, Category = "Tab List")
	void SetTabHiddenState(FName TabNameId, bool bHidden);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool RegisterDynamicTab(const F<PROJECT_NAME>TabDescriptor& TabDescriptor);

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsFirstTabActive() const;

	UFUNCTION(BlueprintCallable, Category = "Tab List")
	bool IsLastTabActive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	bool IsTabVisible(FName TabId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tab List")
	int32 GetVisibleTabCount();

	/** Delegate broadcast when a new tab is created. Allows hook ups after creation. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabContentCreated, FName, TabId, UCommonUserWidget*, TabWidget);
	DECLARE_EVENT_TwoParams(U<PROJECT_NAME>TabListWidgetBase, FOnTabContentCreatedNative, FName /* TabId */, UCommonUserWidget* /* TabWidget */);

	/** Broadcasts when a new tab is created. */
	UPROPERTY(BlueprintAssignable, Category = "Tab List")
	FOnTabContentCreated OnTabContentCreated;
	FOnTabContentCreatedNative OnTabContentCreatedNative;

protected:
	// UUserWidget interface
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// End UUserWidget

	virtual void HandlePreLinkedSwitcherChanged() override;
	virtual void HandlePostLinkedSwitcherChanged() override;

	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

private:
	void SetupTabs();

	UPROPERTY(EditAnywhere, meta=(TitleProperty="TabId"))
	TArray<F<PROJECT_NAME>TabDescriptor> PreregisteredTabInfoArray;
	
	/**
	 * Stores label info for tabs that have been registered at runtime but not yet created. 
	 * Elements are removed once they are created.
	 */
	UPROPERTY()
	TMap<FName, F<PROJECT_NAME>TabDescriptor> PendingTabLabelInfoMap;
};
