#include "Dungeons.h"
#include "EndlessStruggleTiers.h"
#include "MutableEndlessStruggleTier.h"

#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "Algo/Reverse.h"

#include "game/component/MissionProgressComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/difficulty/DifficultyUtil.h"
#include "game/mission/MissionDefs.h"
#include "game/util/ValueFormat.h"
#include "game/EndGame/EndGameContentDefs.h"

#define LOCTEXT_NAMESPACE "EndlessStruggle"

namespace endlesstruggle { namespace tier {
	
	auto struggleTiers = Util::createTArrayOfReservedSize<Unique<MutableEndlessStruggleTier>>(32);

	MutableEndlessStruggleTier& createTier() {
		auto index = struggleTiers.Add(make_unique<MutableEndlessStruggleTier>());
		return *struggleTiers[index];
	}	

	MutableEndlessStruggleTier& createBaseRequirements() {
		return createTier()
			.unlockRequirementsPredicate([](const UMissionProgressComponent* progress) {
				if (auto def = endgamecontent::defs().getChecked(EEndGameContentType::EndlessStruggle)) {
					auto serialize = progress->GetCharacterSerializeComponent();
					return def->isUnlocked(serialize);
				}
				return false;
			})
			.unlockTextProvider([]() {
				if (auto def = endgamecontent::defs().getChecked(EEndGameContentType::EndlessStruggle)) {
					return def->getUnlockRequirementText().Get(FText::GetEmpty());
				}
				return FText::GetEmpty();
			})
			.progressGeneratorProvider([](UMissionProgressComponent* progress) {
				progress->SetCompletedMission(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle, { 0 }, ELevelNames::obsidianpinnacle, 0);
			});
	}

	bool hasBossForEndlessStruggle(const UMissionProgressComponent* progress, const MissionDef& def) {
		return !def.isTrial()
			&& def.hasBoss()
			&& progress->IsMissionRevealed(difficultyquery::RequiredForEndlessStuggle, def.level());
	}

	TArray<const MissionDef*> getAvailableBossMissions(const UMissionProgressComponent* progress) {
		return algo::copy_if(missions::getAllCurrentlyEnabled(), RETLAMBDA(hasBossForEndlessStruggle(progress, *it)));
	}

	int countDefeatedBosses(const UMissionProgressComponent* progress, FEndlessStruggle requiredEndlessStruggle) {
		return algo::count_if(getAvailableBossMissions(progress), RETLAMBDA(progress->IsMissionDifficultyAndThreatLevelCompleted(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle, requiredEndlessStruggle, it->level())));
	}
	
	MutableEndlessStruggleTier& createRequireBeatBosses(FEndlessStruggle requiredEndlessStruggle, int amount) {
		return createTier()
			.unlockRequirementsPredicate([requiredEndlessStruggle, amount](const UMissionProgressComponent* progress) {
				return countDefeatedBosses(progress, requiredEndlessStruggle) >= amount;
			})
			.missionInterestPredicate([requiredEndlessStruggle](const UMissionProgressComponent* progress, const MissionDef& def, EMissionInterest interestType) {
				if (interestType == EMissionInterest::NEW_BOSS){
					return hasBossForEndlessStruggle(progress, def)
						&& !def.isDisabled()
						&& !progress->IsMissionDifficultyAndThreatLevelCompleted(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle, requiredEndlessStruggle, def.level());
				};
				return false;
			})
			.unlockTextProvider([requiredEndlessStruggle, amount]() {
				if (amount == 1){
					return FText::Format(
						LOCTEXT("EndlessStruggleTier_DefeatAnyBossMissionOnXDifficulty", "Complete any boss mission on {0}"),
						UDifficultyUtil::getDifficultyEndlessStruggleDisplayName(difficultyquery::RequiredForEndlessStuggle, { requiredEndlessStruggle })
					);
				} else {
					return FText::Format(
						LOCTEXT("EndlessStruggleTier_DefeatXBossMissionsOnYDifficulty", "Complete {0} boss missions on {1}"),
						valueformat::asNumber(amount),
						UDifficultyUtil::getDifficultyEndlessStruggleDisplayName(difficultyquery::RequiredForEndlessStuggle, { requiredEndlessStruggle })
					);
				}
			})
			.progressTextProvider([requiredEndlessStruggle, amount](const UMissionProgressComponent* progress) {
				return valueformat::asCounter(countDefeatedBosses(progress, requiredEndlessStruggle), amount);
			})
			.progressGeneratorProvider([requiredEndlessStruggle, amount](UMissionProgressComponent* progress) {
				static Random rnd;
				//Complete a number of random boss missions
				auto bossMissions = getAvailableBossMissions(progress);
				for (int i : Util::randomIndices(bossMissions.Num(), FMath::Min(amount, bossMissions.Num()))) {
					progress->SetCompletedMission(difficultyquery::RequiredForEndlessStuggle, threatquery::RequiredForEndlessStuggle, requiredEndlessStruggle, bossMissions[i]->level(), 0);
				}				
			});
	}

	const EndlessStruggleTier& tier0 = createBaseRequirements()
		.unlockedEndlessStruggle({ 3 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER0);

	const EndlessStruggleTier& tier1 = createRequireBeatBosses(tier0.unlockedEndlessStruggle(), 1)
		.unlockedEndlessStruggle({ 6 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER1);

	const EndlessStruggleTier& tier2 = createRequireBeatBosses(tier1.unlockedEndlessStruggle(), 2)
		.unlockedEndlessStruggle({ 9 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER2);

	const EndlessStruggleTier& tier3 = createRequireBeatBosses(tier2.unlockedEndlessStruggle(), 3)
		.unlockedEndlessStruggle({ 12 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER3);

	const EndlessStruggleTier& tier4 = createRequireBeatBosses(tier3.unlockedEndlessStruggle(), 4)
		.unlockedEndlessStruggle({ 15 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER4);

	const EndlessStruggleTier& tier5 = createRequireBeatBosses(tier4.unlockedEndlessStruggle(), 5)
		.unlockedEndlessStruggle({ 18 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER5);

	const EndlessStruggleTier& tier6 = createRequireBeatBosses(tier5.unlockedEndlessStruggle(), 6)
		.unlockedEndlessStruggle({ 21 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER6);

	const EndlessStruggleTier& tier7 = createRequireBeatBosses(tier6.unlockedEndlessStruggle(), 7)
		.unlockedEndlessStruggle({ 25 })
		.progressStat(EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER7);


	TArray<const EndlessStruggleTier*> _getAll() {
		TArray<const EndlessStruggleTier*> all;
		for (auto&& mutabledef : struggleTiers) {
			auto* tier = mutabledef.get();
			all.Add(tier);
		}
		return all;
	}

	const TArray<const EndlessStruggleTier*>& getAllLowestToHighest() {
		static const TArray<const EndlessStruggleTier*> lowestToHighest = []() {
			TArray<const EndlessStruggleTier*> all = _getAll();			
			all.Sort([](const EndlessStruggleTier& a, const EndlessStruggleTier& b){
				return a.unlockedEndlessStruggle() < b.unlockedEndlessStruggle();
			});
			return all;
		}();
		return lowestToHighest;
	}

	const TArray<const EndlessStruggleTier*>& getAllHighestToLowest() {
		static const TArray<const EndlessStruggleTier*> highestToLowest = []() {
			TArray<const EndlessStruggleTier*> all = _getAll();
			all.Sort([](const EndlessStruggleTier& a, const EndlessStruggleTier& b) {
				return a.unlockedEndlessStruggle() > b.unlockedEndlessStruggle();
			});
			return all;
		}();
		return highestToLowest;
	}

	TArray<const EndlessStruggleTier*> getAllUnlocked(const UMissionProgressComponent* progress) {
		return algo::copy_if(getAllLowestToHighest(), RETLAMBDA(it->IsUnlocked(progress)));
	}

	const EndlessStruggleTier* getHighestUnlocked(const UMissionProgressComponent* progress) {				
		for (const auto* tier : getAllHighestToLowest()) {
			if (tier->IsUnlocked(progress)) {
				return tier;
			}
		}
		return nullptr;
	}

	const EndlessStruggleTier* getLowestLocked(const UMissionProgressComponent* progress) {
		for (const auto* tier : getAllLowestToHighest()) {
			if (!tier->IsUnlocked(progress)) {
				return tier;
			}
		}
		return nullptr;
	}

	const EndlessStruggleTier* getUnlockFor(FEndlessStruggle struggle) {		
		for (const auto* tier : getAllLowestToHighest()) {
			if (struggle <= tier->unlockedEndlessStruggle()) {
				return tier;
			}
		}
		return nullptr;
	}

	const EndlessStruggleTier* getTierFromStat(EProgressStat stat) {
		for (const auto* tier : getAllLowestToHighest()) {
			if (tier->progressStat() == stat) {
				return tier;
			}
		}
		return nullptr;
	}

}}

#undef LOCTEXT_NAMESPACE
