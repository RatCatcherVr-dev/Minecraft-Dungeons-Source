#pragma once

// #D11.CM

#include "Engine.h"
#include "Dungeons.h"
#include "VirtualKeyboardManager.generated.h"

class UVirtualKeyboardManager;

class FDungeonsVirtualKeyboardEntry : public IVirtualKeyboardEntry
{
	FDungeonsVirtualKeyboardEntry(UVirtualKeyboardManager&);
public:
	// Used to correctly create our keyboard entry and pass in the right handler.
	static TSharedRef<FDungeonsVirtualKeyboardEntry> Create(UVirtualKeyboardManager&);

	void SetTextFromVirtualKeyboard(const FText&, ETextEntryType) override;

	EKeyboardType GetVirtualKeyboardType() const override { return EKeyboardType::Keyboard_Default; };

	void SetSelectionFromVirtualKeyboard(int SelStart, int SelEnd) override {}

	FText GetText() const override;

	FText GetHintText() const override;

	FVirtualKeyboardOptions GetVirtualKeyboardOptions() const override;

	bool IsMultilineEntry() const override;

	void SetCachedText(FText text) { CachedText = std::move(text); }

private:
	UVirtualKeyboardManager* OwningManager;
	FText CachedText;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextReceivedDelegate, FText, TextInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputCancelledDelegate);

UCLASS(Blueprintable)
class UVirtualKeyboardManager : public UObject
{
	GENERATED_BODY()
public:
	UVirtualKeyboardManager();
	~UVirtualKeyboardManager();

	// This is our text received delegate. This will fire whenever our VirtualKeyboardEntry is closed with positive input.
	UPROPERTY(BlueprintAssignable, Category = "VirtualKeyboard")
	FTextReceivedDelegate OnTextReceived;

	// Our InputCancelled delegate gets called when our VirtualKeyboardEntry is closed through any means other than accepting the input.
	UPROPERTY(BlueprintAssignable, Category = "VirtualKeyboard")
	FInputCancelledDelegate OnInputCancelled;

	// Calling this brings up our VirtualKeyboardEntry.
	// Entering Current Text will set the Entry's cached text.
	UFUNCTION(BlueprintCallable, Category = "VirtualKeyboard")
	void ShowVirtualKeyboard(FText currentText = FText::GetEmpty());
private:
	void SetTextFromVirtualKeyboard(const FText&, ETextEntryType);

	// Called once our keyboard has been closed down by the system, this is where we handle our results.
	friend void FDungeonsVirtualKeyboardEntry::SetTextFromVirtualKeyboard(const FText&, ETextEntryType);

	// This is our virtual keyboard entry, where we interface with the engine.
	TSharedPtr<FDungeonsVirtualKeyboardEntry> VirtualKeyboardEntry;
};
