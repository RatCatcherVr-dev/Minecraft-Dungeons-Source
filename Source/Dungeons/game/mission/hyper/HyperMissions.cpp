#include "Dungeons.h"
#include "HyperMissions.h"
#include "DungeonEligibility.h"
#include "game/item/ItemBulletPoint.h"
#include "game/mission/MissionDLCUtil.h"
#include "game/mission/MissionData.h"
#include "game/util/ValueFormat.h"

namespace missions { namespace hajper {

namespace internal {

float calculateAncientProbability(int ancientDungeonTypes, int enchantmentPoints) {
	static constexpr float goldProb = 6;
	static constexpr float baseAncientProb = 0.5f;
	static constexpr float bonusProbPerAncientDungeon = 0.5f;
	static constexpr float bonusProbPerEnchantmentPoint = 1.0f;

	const float ancientProbability = 
		(baseAncientProb) +
		(ancientDungeonTypes * bonusProbPerAncientDungeon) +
		(enchantmentPoints * bonusProbPerEnchantmentPoint);

	return ancientDungeonTypes?
		ancientProbability / (ancientProbability + goldProb) :
		0;
};
}

float AncientProbability::probabilityMultiplier() const {
	static const float DefaultProbability = internal::calculateAncientProbability(1, 0);
	return probability / DefaultProbability;
}

AncientProbability getAncientProbability(int ancientDungeons, int enchantmentPoints) {
	return { internal::calculateAncientProbability(ancientDungeons, enchantmentPoints) };
}

AncientProbability getAncientProbability(const FMissionState& missionState) {
	return getAncientProbability(
		getNumAncientMobTypes(missionState),
		missionState.getTotalEnchantmentPoints());
}

namespace internal {
	static constexpr float ASSUMED_DUNGEONS = 1.41f;
	static constexpr float NAIVE_TO_PESSIMISTIC_CHANCE_CONVERSION = 0.95f;
}

float getApproximateChanceForAnyAncient(int submissionsLeft, int ancientDungeonsTypes, int sacrificedEnchantmentPoints) {
	const float submissions = static_cast<float>(submissionsLeft);
	const float naiveChance = 1.0f - FMath::Pow(1.0 - getAncientProbability(ancientDungeonsTypes, sacrificedEnchantmentPoints).probability, submissions * internal::ASSUMED_DUNGEONS);
	const float pessimisticChance = internal::NAIVE_TO_PESSIMISTIC_CHANCE_CONVERSION * naiveChance;
	return pessimisticChance;
}

float getApproximateChanceForAnyAncient(const FMissionState& missionState) {
	return getApproximateChanceForAnyAncient(getAncientHuntNumSubMissionsRemaining(missionState), getNumAncientMobTypes(missionState), missionState.getTotalEnchantmentPoints());
}

float getApproximateAverageAmountOfAncients(int submissionsLeft, int ancientDungeonsTypes, int sacrificedEnchantmentPoints) {
	const float submissions = static_cast<float>(submissionsLeft);
	const float naiveAverage = getAncientProbability(ancientDungeonsTypes, sacrificedEnchantmentPoints).probability * submissions * internal::ASSUMED_DUNGEONS;
	const float pessimisticAverage = internal::NAIVE_TO_PESSIMISTIC_CHANCE_CONVERSION * naiveAverage;
	return pessimisticAverage;
}

float getApproximateAverageAmountOfAncients(const FMissionState& missionState) {
	return getApproximateAverageAmountOfAncients(getAncientHuntNumSubMissionsRemaining(missionState), getNumAncientMobTypes(missionState), missionState.getTotalEnchantmentPoints());
}

int getNumAncientMobTypes(ELevelNames mission, const FEligibleDLC& eligibleDLC, const ItemArchetypeCounts& archetypeCounts) {
	return get(mission).countNumAncientMobTypes(getHyperDungeonLevelGenerationPredicate(eligibleDLC, archetypeCounts));
}

int getNumAncientMobTypes(const FMissionState& missionState) {
	return getNumAncientMobTypes(missionState.getLevelName(), missionState.getEligibleDLCs(), missionState.getItemArchetypeCounts());
}


int getAncientHuntNumSubMissions() {
	return 3;
}

int getAncientHuntNumSubMissionsRemaining(const FMissionState& state) {
	return FMath::Max(0, getAncientHuntNumSubMissions() - state.partsDiscovered);
}

int getAncientHuntNumLives(const FMissionState& state) {
	return difficultyquery::getLivesForDifficulty(state.getDifficulty());
}

int getAncientHuntNumLivesRemaining(const FMissionState& state) {
	return FMath::Max(0, getAncientHuntNumLives(state) - state.livesLost);
}

bool isAncientHuntDepleted(const FMissionState& missionState) {
	return
		getAncientHuntNumSubMissionsRemaining(missionState) <= 0 ||
		getAncientHuntNumLivesRemaining(missionState) < 0;
}

void populateMissionBulletPoints(const FMissionState& state, TArray<FItemBulletPoint>& bullets) {
	if (state.partsDiscovered > 0) {
		bullets.Add(FItemBulletPoint::CreateLabeledCounterBulletPoint(NSLOCTEXT("hypermission", "submissions_remaining", "Sub-missions Remaining"), getAncientHuntNumSubMissionsRemaining(state), getAncientHuntNumSubMissions()));
	} else {
		bullets.Add(FItemBulletPoint::CreateLabeledCounterBulletPoint(NSLOCTEXT("hypermission", "submissions", "Sub-missions"), getAncientHuntNumSubMissionsRemaining(state)));
	}

	if (state.livesLost > 0) {
		bullets.Add(FItemBulletPoint::CreateLabeledCounterBulletPoint(NSLOCTEXT("hypermission", "lives_remaining", "Lives Remaining"), getAncientHuntNumLivesRemaining(state), getAncientHuntNumLives(state)));
	} else {
		bullets.Add(FItemBulletPoint::CreateLabeledCounterBulletPoint(NSLOCTEXT("hypermission", "lives", "Lives"), getAncientHuntNumLivesRemaining(state)));
	}
}


void populateMissionProbabilities(const FMissionState& missionState, TArray<FMissionProbability>& probabilities) {	
	const float chanceForAny = getApproximateChanceForAnyAncient(missionState);
	probabilities.Add(FMissionProbability{
		EInfoChanceIcon::None,
		FText::FromString(valueformat::asPercentageChance(chanceForAny)),
		FItemBulletPoint::GetAnyAncientChanceText(chanceForAny),
		});
	
	const float averageAmount = getApproximateAverageAmountOfAncients(missionState);
	probabilities.Add(FMissionProbability{
		EInfoChanceIcon::Average,
		FText::FromString(valueformat::asSingleDecimalConstant(averageAmount)),
		FItemBulletPoint::GetAverageAncientsText(averageAmount),
		});
}

bool shouldIncludeInLevelGeneration(DungeonEligibility dungeonEligibility) {
	return dungeonEligibility == DungeonEligibility::CAN_PLAY_DUNGEON;
}

bool shouldIncludeInUI(DungeonEligibility dungeonEligibility) {
	return dungeonEligibility != DungeonEligibility::CANNOT_PLAY_DUNGEON;
}

io::HyperDungeonPredicate getHyperDungeonLevelGenerationPredicate(const FEligibleDLC& eligibleDLC, const ItemArchetypeCounts& archetypeCounts) {	
	return [eligibilityPredicate = game::mission::dlc::getDungeonEligibilityPredicate(eligibleDLC, archetypeCounts)]
		(const io::HyperDungeon& dungeon) { 
			return shouldIncludeInLevelGeneration(eligibilityPredicate(dungeon)); 
		};
}

io::HyperDungeonPredicate getHyperDungeonLevelGenerationPredicate(const FMissionState& missionState) {
	return getHyperDungeonLevelGenerationPredicate(missionState.getEligibleDLCs(), missionState.getItemArchetypeCounts());
}


}}
