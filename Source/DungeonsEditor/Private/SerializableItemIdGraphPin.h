#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphPin.h"
#include "SComboBox.h"

class SComboButton;

class SSerializableItemIdGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SSerializableItemIdGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface
private:

	TSharedRef<SWidget> OnGenerateWidget(const TSharedPtr<FString>);
	void OnSelectionChanged(TSharedPtr<FString> InSelectedItem, ESelectInfo::Type SelectInfo);
	
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;
	TSharedPtr<STextBlock> SelectionWidget;
	TArray<TSharedPtr<FString>> ListEntries;
	TArray<TSharedPtr<SToolTip>> ToolTips;

	TSharedPtr<FString> ParseCurrentselection();
};