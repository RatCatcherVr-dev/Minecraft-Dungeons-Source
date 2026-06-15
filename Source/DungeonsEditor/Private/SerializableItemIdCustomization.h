#pragma once

#include "SToolTip.h"
#include "IPropertyTypeCustomization.h"

class FSerializableItemIdCustomization: public IPropertyTypeCustomization
{
	
	void OnGetStrings(TArray< TSharedPtr< FString > > &, TArray< TSharedPtr< SToolTip > > &, TArray< bool > &);
	FString GetValue(TSharedRef<IPropertyHandle> PropertyHandle);
	void OnValueSelected(const FString& string, TSharedRef<IPropertyHandle> PropertyHandle);
public:
    // IDetailCustomization interface
    void CustomizeHeader( TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils ) override;
	void CustomizeChildren( TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {};
  

    static TSharedRef< IPropertyTypeCustomization > MakeInstance();
};