#include "Dungeons.h"
#include "MockTrials.h"
#include "TrialUtil.h"
#include "game/GameSettings.h"
#include "game/affector/AffectorTypes.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/mission/MissionDefs.h"
#include "util/Random.h"
#include "util/RandomUtil.h"

//
// Generate Trials
//
namespace trial { namespace mock {

affector::RuleData generateRandomAffectors(Random& rnd) {
	affector::RuleData out;

	const auto& affectorTypes = affector::getTypes();
	auto indices = Util::randomIndices(affectorTypes.Num(), &rnd);

	const size_t maxGuaranteedHarderCount = 1;
	const size_t maxTotalCount = FMath::Min(maxGuaranteedHarderCount + 2, indices.size());
	const size_t harderCount = std::distance(indices.begin(), std::stable_partition(indices.begin(), indices.end(), [&, pickedHard = 0](size_t i) mutable {
		return affectorTypes[i]->HasDataGenerator(EAffectorModus::Harder) && ++pickedHard <= maxGuaranteedHarderCount;
	}));

	for (size_t i = 0; i < maxTotalCount; ++i) {
		const auto& affectorType = *affectorTypes[indices[i]];
		const auto modus = i < harderCount ? EAffectorModus::Harder : EAffectorModus::Any;
		const auto preferEasier = rnd.testProbability(0.60f);
		out.Set(affectorType.Id, affectorType.GetDataGenerator(modus, preferEasier)({ rnd }));
	}
	return out;
}

TArray<FMissionItemChance> generateRandomRewards(Random& rnd) {
	TArray<FMissionItemChance> out;

	const auto randomRarity = [&rnd](float common = 1.0f, float rare = 1.0f, float unique = 1.0f) {
		return Util::randomWeightedItem<EItemRarity>({{common, EItemRarity::Common }, {rare, EItemRarity::Rare }, {unique, EItemRarity::Unique }}, &rnd);
	};

	if (rnd.testProbability(0.4f)) {
		const auto tag = Util::randomWeightedItem<ItemTag>({
			{1, ItemTag::Armor},
			{1, ItemTag::MeleeWeapon},
			{1, ItemTag::RangedWeapon},
		}, &rnd);
		out.Add({ {}, FMissionItemTagChance { tag }, {} });
	}
	if (rnd.testProbability(0.5f)) {
		out.Add({ {}, {}, FMissionItemRarityChance { randomRarity() } });
	}
	if (out.Num() == 0) {
		const itemgen::Config config(RETLAMBDA(it.item.isUnique()));
		const auto uniques = getPossibleItemTypeIds({}, itemgen::looters::Unknown(), config);
		if (const auto unique = Util::randomChoiceOrEmpty(uniques, &rnd)) {
			out.Add({ FMissionItemTypeChance { unique.GetValue(), EItemRarity::Unique }, {}, {} });
		}
	}
	return out;
}

EExtraChallenge generateExtraChallenge(Random& rnd) {
	const int challengeAmount = rnd.nextInt(NumberOfExtraChallenges) + 1;
	return extrachallengequery::getExtraChallengeFromAmount(challengeAmount);
}

FTrialDef generateRandom(ELevelNames level, ELevelVariationType variationType, RandomSeed seed) {
	Random rnd(seed);

	FTrialDef out{ level };
	out.extraChallenge = generateExtraChallenge(rnd);
	out.affectors = generateRandomAffectors(rnd);
	out.rewards = generateRandomRewards(rnd);
	out.type = GetEnumValueToStringStripped(variationType);
	return out;
}

TArray<FTrialDef> generateRandom(const RandomSeed baseSeed, TOptional<int> count) {
	Random rnd(baseSeed);

	// When we have anything more than daily trials, we will probably have a backend as well
	const auto all = allTrials();
	const int desiredCountToGenerate = count.Get(all.Num());
	const auto randomIndices = Util::randomIndices(all.Num(), FMath::Clamp(desiredCountToGenerate, 0, all.Num()), &rnd);
	return algo::map_tarray(randomIndices, [&](auto i) {
		const auto level = all[i]->level();
		const auto VariationType = all[i]->levelVariation();
		return generateRandom(level, VariationType, baseSeed + 1337 * enum_cast(level));
	}); 
}

}}
