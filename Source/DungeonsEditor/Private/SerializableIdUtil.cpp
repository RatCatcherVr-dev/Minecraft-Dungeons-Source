#include "SerializableIdUtil.h"


// #define UNIQUEID_EXCLUDE_STATIC //Hack tiem
#include "game/UniqueId.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"

void PopulateComboBoxData(TArray<TSharedPtr<FString>>& strings, TArray<TSharedPtr<SToolTip>>& tooltips, TArray<bool>& valid) {
	 for (auto itemType : GetItemRegistry().GetValues()) {
	 	strings.Emplace(MakeShared<FString>(itemType->getId().GetBackingType().ToString()));
	 }

	strings.Sort([](const TSharedPtr<FString> a, const TSharedPtr<FString> b) { return *a < *b; });
	
	for (auto string : strings) {
		if (const auto id = GetItemRegistry().Request(**string)) {
			const auto& itemType = GetItemRegistry().Get(id.GetValue());
			tooltips.Emplace(SNew(SToolTip).Text(itemType.getNameText()));
		}
		else {
			tooltips.Emplace(SNew(SToolTip));
		}
	}
	
}