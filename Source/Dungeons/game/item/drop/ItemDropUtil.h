#pragma once
#include "ItemDropData.h"

namespace game { namespace item { namespace drop {

bool isGearOrPermanent(EDropGeneratorCategory);

TArray<FNetworkedItemDropData> toHyperMissionDrops(const TArray<FNetworkedItemDropData>&, EItemRarityChanceCategory, const AActor* sourceActor);

}}}
