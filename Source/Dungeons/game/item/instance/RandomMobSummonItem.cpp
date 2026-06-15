// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "RandomMobSummonItem.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "util/WeighedRandom.h"
#include "game/GameTypes.h"
#include "game/component/RangedAttackComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/ComponentUtils.h"

EntityType ARandomMobSummonItem::GetMobType() const {
	check(MobsToChooseFrom.Num() > 0);
	std::vector<FMobChoice> choices;
	algo::map_to(MobsToChooseFrom, RETLAMBDA(it), choices);

	return WeighedRandom::getRandomItem(&Util::sharedRandom(), choices)->MobType;
}

float ARandomMobSummonItem::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
	{
		float totalWeight = algo::sum(MobsToChooseFrom, [](const auto& choice) { return choice.getWeight(); });
		return algo::sum(MobsToChooseFrom, [totalWeight, stat](const auto& choice) {
			return AMobSummonItem::GetStatsForEntityType(stat, choice.MobType)* choice.getWeight() / totalWeight;
		});
	}
	}

	return Super::GetStats(stat);
}