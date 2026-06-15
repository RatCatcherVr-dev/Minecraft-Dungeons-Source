#include "SerializableItemIdCustomization.h"

#include "common_header.h"
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "components/SkeletalMeshComponent.h"
#include "SerializableIdUtil.h"


#define LOCTEXT_NAMESPACE "SerializableItemIdCustomization"


TSharedRef<IPropertyTypeCustomization> FSerializableItemIdCustomization::MakeInstance() {
	 return MakeShareable(new FSerializableItemIdCustomization);
}

void FSerializableItemIdCustomization::OnGetStrings(TArray<TSharedPtr<FString>>& strings, TArray<TSharedPtr<SToolTip>>& tooltips, TArray<bool>& valid) {
	PopulateComboBoxData(strings, tooltips, valid);
}

FString FSerializableItemIdCustomization::GetValue(TSharedRef<IPropertyHandle> PropertyHandle) {
	if(const TSharedPtr< IPropertyHandle > ChildHandle = PropertyHandle->GetChildHandle(FName(TEXT("SerializedId")))) {
		FName value;
		ChildHandle->GetValue(value);
		return value.ToString();
	}

	return "ERROR";
}
void FSerializableItemIdCustomization::OnValueSelected(const FString& string, TSharedRef<IPropertyHandle> PropertyHandle) {
	if (const TSharedPtr< IPropertyHandle > ChildHandle = PropertyHandle->GetChildHandle(FName(TEXT("SerializedId")))) {
		ChildHandle->SetValue(FName(*string));
	}
}


void FSerializableItemIdCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) {
	const auto getStrings = FOnGetPropertyComboBoxStrings::CreateSP(SharedThis(this), &FSerializableItemIdCustomization::OnGetStrings);
	const auto getValue = FOnGetPropertyComboBoxValue::CreateSP(SharedThis(this), &FSerializableItemIdCustomization::GetValue, PropertyHandle);
	const auto valueSelected = FOnPropertyComboBoxValueSelected::CreateSP(SharedThis(this), &FSerializableItemIdCustomization::OnValueSelected, PropertyHandle);

	HeaderRow.NameContent().Widget = PropertyHandle->CreatePropertyNameWidget();
	HeaderRow.ValueContent().MinDesiredWidth(300).Widget = PropertyCustomizationHelpers::MakePropertyComboBox(PropertyHandle, getStrings, getValue, valueSelected);
}

#undef LOCTEXT_NAMESPACE