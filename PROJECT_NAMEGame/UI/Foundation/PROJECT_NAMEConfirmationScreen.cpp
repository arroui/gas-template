// Copyright Epic Games, Inc. All Rights Reserved.

#include "<PROJECT_NAME>ConfirmationScreen.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

#include "CommonBorder.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "ICommonInputModule.h"
#include "Input/Reply.h"
#include "CommonButtonBase.h"
#include "<PROJECT_NAME>ButtonBase.h"

void U<PROJECT_NAME>ConfirmationScreen::SetupDialog(UCommonGameDialogDescriptor* Descriptor, FCommonMessagingResultDelegate ResultCallback)
{
	Super::SetupDialog(Descriptor, ResultCallback);

	Text_Title->SetText(Descriptor->Header);
	RichText_Description->SetText(Descriptor->Body);

	EntryBox_Buttons->Reset<U<PROJECT_NAME>ButtonBase>([](U<PROJECT_NAME>ButtonBase& Button)
	{
		Button.OnClicked().Clear();
	});

	for (const FConfirmationDialogAction& Action : Descriptor->ButtonActions)
	{
		FDataTableRowHandle ActionRow;

		switch(Action.Result)
		{
			case ECommonMessagingResult::Confirmed:
				ActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();
				break;
			case ECommonMessagingResult::Declined:
				ActionRow = ICommonInputModule::GetSettings().GetDefaultBackAction();
				break;
			case ECommonMessagingResult::Cancelled:
				ActionRow = CancelAction;
				break;
			default:
				ensure(false);
				continue;
		}

		U<PROJECT_NAME>ButtonBase* Button = EntryBox_Buttons->CreateEntry<U<PROJECT_NAME>ButtonBase>();
		Button->SetTriggeringInputAction(ActionRow);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = ResultCallback;
}

void U<PROJECT_NAME>ConfirmationScreen::KillDialog()
{
	Super::KillDialog();
}

void U<PROJECT_NAME>ConfirmationScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Border_TapToCloseZone->OnMouseButtonDownEvent.BindDynamic(this, &U<PROJECT_NAME>ConfirmationScreen::HandleTapToCloseZoneMouseButtonDown);
}

void U<PROJECT_NAME>ConfirmationScreen::CloseConfirmationWindow(ECommonMessagingResult Result)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(Result);
}

FEventReply U<PROJECT_NAME>ConfirmationScreen::HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = FReply::Unhandled();

	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CloseConfirmationWindow(ECommonMessagingResult::Declined);
		Reply.NativeReply = FReply::Handled();
	}

	return Reply;
}

#if WITH_EDITOR
void U<PROJECT_NAME>ConfirmationScreen::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	if (CancelAction.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has unset property: CancelAction.")), FText::FromString(GetName())));
	}
}
#endif