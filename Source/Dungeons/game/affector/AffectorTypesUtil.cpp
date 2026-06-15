#include "Dungeons.h"
#include "AffectorTypesUtil.h"
#include "AffectorData.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "util/StringUtil.h"
#include <LogMacros.h>

namespace affector { namespace parse {

template <typename TargetType>
TOptional<TargetAndProbability<TargetType>> _parseProbabilityTargetFormat(const FAffectorData& data, const FString& logPrefix) {
	const auto probabilityTarget = typeWeightPair<FString, TargetType>(data);

	if (!probabilityTarget) {
		UE_LOG(LogDungeons, Error, TEXT("Affectors: Couldn't parse %s: Data is missing the '->' symbol: %s"), *logPrefix, *data.AsString());
		return {};
	}
	if (!probabilityTarget->first) {
		UE_LOG(LogDungeons, Error, TEXT("Affectors: Couldn't parse %s. Incorrect probability format: '%s'"), *logPrefix, *data.AsString());
		return {};
	}
	if (!probabilityTarget->second || !probabilityTarget->second->type) {
		UE_LOG(LogDungeons, Error, TEXT("Affectors: Couldn't parse %s. Incorrect target: '%s'"), *logPrefix, *data.AsString());
		return {};
	}

	return TargetAndProbability<TargetType>{ probabilityTarget->second->type.GetValue(), probabilityTarget->first->weight };
}


TOptional<ReplaceMobTypeInfo> replaceMobTypeInfo(const FAffectorData& data) {
	// Example: "0.5 -> creeper" means: with 0.5 (== 50%) probability, convert the type to a creeper
	return _parseProbabilityTargetFormat<ReplaceMobTypeInfo::TargetType>(data, "Replace*Mob");
}

TOptional<MobEnchantmentInfo> mobEnchantmentInfo(const FAffectorData& data) {
	// Example: "0.5 -> Exploding" means: with 0.5 (== 50%) probability, we apply the Exploding enchantment
	return _parseProbabilityTargetFormat<MobEnchantmentInfo::TargetType>(data, "MobEnchantment");
}

}}
