#include "Dungeons.h"
#include "ArmorPropertyData.h"
#include "ArmorPropertyType.h"
#include "ArmorPropertyTypeDefs.h"
#include "ArmorPropertiesUtil.h"
#include "util/SharedRandom.h"
#include "util/RandomUtil.h"
#include "util/CollectionUtils.h"

namespace game { namespace armorproperties {

TArray<FArmorPropertyData> generateRandomArmorProperties(int positivePropertiesCount, const TSet<ELevelNames>& unlockedLevels) {
	TArray<ArmorPropertyType> validPositiveProperties = game::armorproperties::type::getArmorPropertiesOfClassification(EArmorPropertyClassification::Positive)
		.FilterByPredicate(
			[unlockedLevels](ArmorPropertyType type) {
				if (type.getRandomGenerationPrevented()) { return false; }
				auto enabledLevels = type.getRandomGenerationEnabledByLevels();
				if (enabledLevels.Num() <= 0) { return true; }
				if (enabledLevels.Num() > 0 && unlockedLevels.Num() <= 0) { return false; }
				for (auto enabledLevel : enabledLevels) { if (unlockedLevels.Contains(enabledLevel)) { return true; } }
				return false;
			});
	ensure(positivePropertiesCount > 0 && positivePropertiesCount <= validPositiveProperties.Num());

	TArray<EArmorPropertyID> propertyIDs;
	for (int index : Util::randomIndices(validPositiveProperties.Num(), positivePropertiesCount)) {
		propertyIDs.Add(validPositiveProperties[index].getId());
	}

	if (Util::sharedRandom().testProbability(0.2f)) {
		TArray<ArmorPropertyType> properties = game::armorproperties::type::getArmorPropertiesOfClassification(EArmorPropertyClassification::Negative);
		propertyIDs.Add(properties[FMath::RandRange(0, properties.Num() - 1)].getId());
	}

	return Util::map(RETLAMBDA(FArmorPropertyData(it)), propertyIDs);
}

}}
