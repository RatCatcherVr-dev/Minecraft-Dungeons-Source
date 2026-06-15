#include "common_header.h"
#include "SerializableItemIdGraphPin.h"
#include "Widgets/Input/SComboButton.h"
#include "K2Node_CallFunction.h"
#include "Widgets/Layout/SScaleBox.h"
#include "K2Node_VariableSet.h"
#include "SerializableIdUtil.h"
#include "game/item/SerializableItemId.h"

#define LOCTEXT_NAMESPACE "SerializableItemIdGraphPin"

void SSerializableItemIdGraphPin::Construct( const FArguments& InArgs, UEdGraphPin* InGraphPinObj )
{
	SGraphPin::Construct( SGraphPin::FArguments(), InGraphPinObj );
}

TSharedRef<SWidget>	SSerializableItemIdGraphPin::GetDefaultValueWidget()
{
	TArray<bool> ValidEntries;
	PopulateComboBoxData(ListEntries, ToolTips, ValidEntries);
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ComboBox, SComboBox<TSharedPtr<FString>>).Content()
			[
				SAssignNew(SelectionWidget,STextBlock)
			]
			.ContentPadding(FMargin(2.0f, 2.0f))
			.OptionsSource(&ListEntries)
			.OnGenerateWidget(this, &SSerializableItemIdGraphPin::OnGenerateWidget)
			.OnSelectionChanged(this, &SSerializableItemIdGraphPin::OnSelectionChanged)
			.InitiallySelectedItem(ParseCurrentselection())
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		];
}

TSharedRef<SWidget> SSerializableItemIdGraphPin::OnGenerateWidget(const TSharedPtr<FString> inString)
{
	auto index = ListEntries.IndexOfByKey(inString);

	auto widget = SNew(STextBlock).Text(FText::FromString(*inString));
	if(index != INDEX_NONE) {
		widget.Get().SetToolTip(ToolTips[index]);
	}
	
	return widget;
}

void SSerializableItemIdGraphPin::OnSelectionChanged(TSharedPtr<FString> InSelectedItem, ESelectInfo::Type SelectInfo) {
	FString IdString(TEXT(""));
	if (InSelectedItem.IsValid() && !InSelectedItem->IsEmpty())
	{
		IdString = FString::Printf(TEXT("(SerializedId=\"%s\")"), **InSelectedItem);

		SelectionWidget->SetText(*InSelectedItem);
	}
	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();
	if (CurrentDefaultValue.IsEmpty())
	{
		CurrentDefaultValue = FString(TEXT(""));
	}
	if (!CurrentDefaultValue.Equals(IdString))
	{
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, IdString);
	}
}

TSharedPtr<FString> SSerializableItemIdGraphPin::ParseCurrentselection() {
	FString IdString = GraphPinObj->GetDefaultAsString();

	if(IdString.IsEmpty()) {
		OnSelectionChanged(MakeShared<FString>(FSerializableItemId().ToString()), ESelectInfo::Direct);
	}

	IdString = GraphPinObj->GetDefaultAsString();
	
	if (IdString.StartsWith(TEXT("(")) && IdString.EndsWith(TEXT(")"))) {
		IdString = IdString.LeftChop(1);
		IdString = IdString.RightChop(1);
		IdString.Split("=", NULL, &IdString);
		if (IdString.StartsWith(TEXT("\"")) && IdString.EndsWith(TEXT("\""))) {
			IdString = IdString.LeftChop(1);
			IdString = IdString.RightChop(1);
		}
	}
	
	if (auto entry = ListEntries.FindByPredicate([&](const auto& v1) { return *v1 == IdString; })) {
		SelectionWidget->SetText(**entry);
		return *entry;
	}

	return MakeShared<FString>("");
}


#undef LOCTEXT_NAMESPACE