#include "Dungeons.h"
#include "PredefinedItemDropComponent.h"

void UPredefinedItemDropComponent::SetDropData(const FItemDrop& dropData) {
	DropData = dropData;
}

TArray<FNetworkedItemDropData> UPredefinedItemDropComponent::GatherItemDropData(const FItemDropSource& dropSource) {
	if (DropData.Category == EDropCategory::Consumable) {
		if (ABaseCharacter* triggeringCharacter = Cast<ABaseCharacter>(dropSource.TriggeringPlayer)) {
			if (!triggeringCharacter->IsUnderwater()) {
				DropData.Category = EDropCategory::ConsumableNoWaterBreathing;
			}
		}
	}

	if (auto converted = FNetworkedItemDropData::FromFItemDrop(DropData)) {
		return { converted.GetValue() };
	}

	return {};
}
