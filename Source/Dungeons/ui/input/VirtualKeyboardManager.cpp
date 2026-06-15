#include "VirtualKeyboardManager.h"

// #D11.CM

FDungeonsVirtualKeyboardEntry::FDungeonsVirtualKeyboardEntry(UVirtualKeyboardManager& InOwningManager)
	: OwningManager(&InOwningManager)
{
}

// Virtual Keyboard //
TSharedRef<FDungeonsVirtualKeyboardEntry> FDungeonsVirtualKeyboardEntry::Create(UVirtualKeyboardManager& InOwningManager)
{
	return MakeShareable(new FDungeonsVirtualKeyboardEntry(InOwningManager));
}

void FDungeonsVirtualKeyboardEntry::SetTextFromVirtualKeyboard(const FText& InNewText, ETextEntryType TextEntryType)
{
	CachedText = InNewText;
	OwningManager->SetTextFromVirtualKeyboard(InNewText, TextEntryType);
}

FText FDungeonsVirtualKeyboardEntry::GetText() const
{
	return CachedText;
}

FText FDungeonsVirtualKeyboardEntry::GetHintText() const
{
	return FText();
}

FVirtualKeyboardOptions FDungeonsVirtualKeyboardEntry::GetVirtualKeyboardOptions() const
{
	return FVirtualKeyboardOptions();
}

bool FDungeonsVirtualKeyboardEntry::IsMultilineEntry() const
{
	return false;
}

// Keyboard Manager //
UVirtualKeyboardManager::UVirtualKeyboardManager()
{
	VirtualKeyboardEntry = FDungeonsVirtualKeyboardEntry::Create(*this);
}

UVirtualKeyboardManager::~UVirtualKeyboardManager()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().ShowVirtualKeyboard(false, 0, VirtualKeyboardEntry);
	}
}

void UVirtualKeyboardManager::ShowVirtualKeyboard(FText currentText /* = FText::GetEmpty()*/)
{
	if (FSlateApplication::IsInitialized())
	{
		VirtualKeyboardEntry->SetCachedText(currentText);
		FSlateApplication::Get().ShowVirtualKeyboard(true, 0, VirtualKeyboardEntry);
	}
}

void UVirtualKeyboardManager::SetTextFromVirtualKeyboard(const FText& InNewText, ETextEntryType TextEntryType)
{
	// This can execute outside of the game thread, so we need to make sure that our delegates are called when the gamethread is active.
	TWeakObjectPtr<UVirtualKeyboardManager> WeakManager = this;
	FFunctionGraphTask::CreateAndDispatchWhenReady([WeakManager, InNewText, TextEntryType]()
	{
		if (WeakManager.IsValid()) {
			if (TextEntryType == ETextEntryType::TextEntryAccepted)
			{
				if (WeakManager->OnTextReceived.IsBound())
				{
					WeakManager->OnTextReceived.Broadcast(InNewText);
				}
			}
			else
			{
				if (WeakManager->OnInputCancelled.IsBound())
				{
					WeakManager->OnInputCancelled.Broadcast();
				}
			}
		}
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}
