#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/EquipmentComponent.h"
#include "ItemGeneratorLooterState.h"

namespace game { namespace item { namespace generator {
	
float Looter::highestItemPowerOf(const FItemId& id) const {
	float maxPower = 0;

	for (const auto& item : inventoryItems) {
		if (id == item.GetItemId() && item.ItemPower > maxPower) {
			maxPower = item.ItemPower;
		}
	}
	return maxPower;
}

//
// Looters
//
namespace looters {

namespace inventories {
	static const TArray<FInventoryItemData> empty;
}

namespace unlockedlevels {
	static const TSet<ELevelNames> none;
}

namespace looterstats {
	static const TOptional<game::DifficultyStats> none;
}

Looter Unknown() {	
	return Looter{ inventories::empty, unlockedlevels::none, nullptr, looterstats::none };
}

Looter FromPlayerCharacter(const APlayerCharacter& playerCharacter) {
	UItemStashComponent* itemStash = playerCharacter.FindComponentByClass<UItemStashComponent>();
	UMissionProgressComponent* missionProgress = playerCharacter.FindComponentByClass<UMissionProgressComponent>();
	UEquipmentComponent* equipment = playerCharacter.FindComponentByClass<UEquipmentComponent>();

	return Looter{
		itemStash->GetAsInventoryDataArray(),
		missionProgress->GetUnlockedMissionsSet(),
		&equipment->GetDifficultyRecommendation(),
		missionProgress->GetHighestCompletedDifficultyStats(),
	};
}

Looter FromConsolePlayerCharacter(const APlayerCharacter& playerCharacter) {
	UItemStashComponent* itemStash = playerCharacter.FindComponentByClass<UItemStashComponent>();
	UMissionProgressComponent* missionProgress = playerCharacter.FindComponentByClass<UMissionProgressComponent>();	

	return Looter{
		itemStash->GetAsInventoryDataArray(),
		missionProgress->GetUnlockedMissionsSet(),
		nullptr,	// DG: could actually get it from character tho, now that we have characters
		missionProgress->GetHighestCompletedDifficultyStats(),
	};
}

}

}}}
