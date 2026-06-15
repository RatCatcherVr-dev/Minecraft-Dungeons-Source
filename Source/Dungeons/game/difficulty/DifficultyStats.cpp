#include "Dungeons.h"
#include "DifficultyStats.h"
#include "game/util/ActorQuery.h"
#include "util/FloatRange.h"
#include "Difficulty.h"
#include "game/Game.h"
#include "game/item/power/ItemPowerStats.h"
#include "lovika/LovikaLevelActor.h"

namespace game {

float calculateMinItemPower(const FDifficulty& difficulty, const TSet<DifficultyStats::EPowerRangeBonus>& itemPowerRangeBonuses, const FEndlessStruggleConfiguration& endlessStruggle, float extraVirtualThreatLevel = 0.0f) {
	float progress = difficulty.globalThreatFraction() + difficulty.endlessStruggleAsGlobalProgressFraction();
	const float numGlobalThreats = (float)FDifficulty::numGlobalThreats();
	//Extra challenge related progress increase
	for (DifficultyStats::EPowerRangeBonus bonus : itemPowerRangeBonuses)
	{
		switch (bonus)
		{
		case DifficultyStats::EPowerRangeBonus::MidGameAffectorsBonus:
		{
			const float tokensCollected = difficulty.emergentDifficulty().GetTotalTokensCollected();
			if (tokensCollected > 0) {
				const float extraThreatLevels = tokensCollected / rewardquery::GetMaxEmergentDifficultyCollectedTokens() * 1.1f;
				progress += extraThreatLevels / numGlobalThreats;
			}
		}
		break;
		case DifficultyStats::EPowerRangeBonus::ExtraChallengeBonus:
		{
			const int extraChallengeIndex = difficulty.extraChallengeIndex();
			if (extraChallengeIndex > 0) {
				const float extraThreatLevels = 0.7f + (extraChallengeIndex - 1) * 0.2f;
				progress += extraThreatLevels / numGlobalThreats;
			}
		}
		break;
		default:
			break;
		}
	}

	//Optional extra virtual threat.
	progress += extraVirtualThreatLevel / numGlobalThreats;

	return game::item::power::GetPowerLevelFromProgressFraction(progress) * endlessStruggle.LootPowerMultiplier;
}

FloatRange calculateItemPowerRange(const FDifficulty& difficulty, const TSet<DifficultyStats::EPowerRangeBonus>& itemPowerRangeBonuses, const FEndlessStruggleConfiguration& endlessStruggle) {
	return {
		calculateMinItemPower(difficulty, itemPowerRangeBonuses, endlessStruggle),
		calculateMinItemPower(difficulty, itemPowerRangeBonuses, endlessStruggle, 1.3f)
	};
}

FEndlessStruggleConfiguration getEndlessStruggleConfig( FDifficulty difficulty ) {
	if( GWorld ) {
		if( auto instance = GWorld->GetGameInstance<UDungeonsGameInstance>() ) {
			if( auto library = instance->GetEndlessStruggleLibrary() ) {
				return library->GetConfiguration( difficulty.endlessStruggle().Value );
			}
		}
	}
	return FEndlessStruggleConfiguration();
}

DifficultyStats::DifficultyStats(FDifficulty difficulty)
	: difficulty(difficulty)
	, difficultyMultiplierOffset(difficulty.combined())
	, endlessStruggleConfiguration(getEndlessStruggleConfig(difficulty))
	, dropItemPower(calculateItemPowerRange(difficulty, {}, endlessStruggleConfiguration))
	, rewardItemPower(calculateItemPowerRange(difficulty, { EPowerRangeBonus::ExtraChallengeBonus }, endlessStruggleConfiguration))
	, raidCaptainChestItemPower(calculateItemPowerRange(difficulty, { EPowerRangeBonus::ExtraChallengeBonus, EPowerRangeBonus::MidGameAffectorsBonus }, endlessStruggleConfiguration))
	, rareItemPowerBoost(0.1f)
	, extraDropItemPowerBonus(0)
	, extraRewardItemPowerBonus(0) {

}




const FDifficulty& DifficultyStats::GetDifficulty() const {
	return difficulty;
}






float DifficultyStats::GetMobMaxHealthMultiplier() const {
	static const float OffsetMultiplierComparedToItemProgression = 1.0f;
	return GetScaledMultiplierAdjustedToNumberOfPlayers(2.0f * OffsetMultiplierComparedToItemProgression, 0.3f, 0.1f) * 
		endlessStruggleConfiguration.MobMaxHealthMultiplier;
}

float DifficultyStats::GetMobPerformHealingMultiplier() const {
	return GetMobMaxHealthMultiplier() * endlessStruggleConfiguration.MobPerformHealingMultiplier;
}

float DifficultyStats::GetMobMaxEnduranceMultiplier() const {
	return GetMobMaxHealthMultiplier() * endlessStruggleConfiguration.MobMaxEnduranceMultiplier;
}

float DifficultyStats::GetMobRecoverEnduranceMultiplier() const {
	return GetMobMaxEnduranceMultiplier() * endlessStruggleConfiguration.MobRecoverEnduranceMultiplier;
}


float DifficultyStats::GetMobDamageMultiplier() const {
	//return GetScaledMultiplierAdjustedToNumberOfPlayers(1.2f, 0.4f, 0.1f);
	static const float OffsetMultiplierComparedToArmorProgression = 1.0f;
	static const float GlobalMobDamage = 0.8f;
	return GetScaledMultiplierAdjustedToNumberOfPlayers(1.0f * OffsetMultiplierComparedToArmorProgression, 0.15f, 0.1f) 
		* GlobalMobDamage
		* endlessStruggleConfiguration.MobDamageMultiplier;
}

float DifficultyStats::GetSpecialMobMaxHealthMultiplier() const {
	return GetMobMaxHealthMultiplier() * GetPlayerCountMobMultiplier();
}

float DifficultyStats::GetMobSpeedMultiplier() const {
	return endlessStruggleConfiguration.MobSpeedMultiplier;
}

float DifficultyStats::GetMobResurrectionChance() const {
	return endlessStruggleConfiguration.MobResurrectionChance;
}

float DifficultyStats::GetMobDeflectChance() const {
	return endlessStruggleConfiguration.MobDeflectChance;
}

float DifficultyStats::GetMobStunDurationMultiplier() const {
	return FMath::Lerp(1.0f, 0.25f, difficulty.globalThreatFraction()) * endlessStruggleConfiguration.MobStunDurationMultiplier;
}

float DifficultyStats::GetMobPushbackMultiplier() const {
	return endlessStruggleConfiguration.MobPushbackMultiplier;
}

float DifficultyStats::GetPlayerCountMobMultiplier() const {
	return 0.7f + GetCappedNumberOfPlayers() * 0.3f;
}

float DifficultyStats::GetEndlessStruggleMobMultiplier() const {
	return endlessStruggleConfiguration.MobCountMultiplier; 
}

float DifficultyStats::GetMobCountMultiplier() const {

	const EThreatLevel MobCountMiddlePointThreatLevel = EThreatLevel::Threat_4;
	//Decrease mobs for the first couple of threat levels.
	const float mobMultiplierBase = [&]() {
		const FDifficulty lower = FDifficulty(difficultyquery::First, threatquery::First);
		const FDifficulty higher = FDifficulty(difficultyquery::First, MobCountMiddlePointThreatLevel);
		return FMath::Lerp(0.5f, 1.0f, difficulty.globalThreatFractionClampedBetweenDifficulties(lower, higher));
	}();

	//Slowly increase mobs to +25% at last map of diamond.
	const float mobMultiplierBonus = [&]() {
		const FDifficulty lower = FDifficulty(difficultyquery::First, MobCountMiddlePointThreatLevel);
		const FDifficulty higher = FDifficulty(difficultyquery::Last, threatquery::Last);
		return FMath::Lerp(0.0f, 0.25f, difficulty.globalThreatFractionClampedBetweenDifficulties(lower, higher));
	}();

	return ( mobMultiplierBase + mobMultiplierBonus ) * GetPlayerCountMobMultiplier() * endlessStruggleConfiguration.MobCountMultiplier;
}





/** Minimum difficulty required to spawn enchanted mobs */
const FDifficulty firstDifficultyWithEnchantedMobs = FDifficulty(difficultyquery::Default, EThreatLevel::Threat_4);
const FDifficulty peakEnchantmentsDifficulty = FDifficulty(difficultyquery::Last, threatquery::Last);

float calculateMobEnchantmentsFraction(const FDifficulty& difficulty) {
	return difficulty.globalThreatFractionClampedBetweenDifficulties(firstDifficultyWithEnchantedMobs, peakEnchantmentsDifficulty);
}

float DifficultyStats::GetEnchantedMobProbability() const {
	float probabiltiy = 0.0f;
	if (difficulty >= firstDifficultyWithEnchantedMobs) {
		probabiltiy += FloatRange(0.05f, 0.1f).lerp(calculateMobEnchantmentsFraction(difficulty));
	}
	probabiltiy += FloatRange(0.0f, 0.05f).lerp(difficulty.extraChallengeFraction());
	//probabiltiy += FloatRange(0.1f, 0.8f).lerp(difficulty.endlessStruggleFraction());
	probabiltiy *= endlessStruggleConfiguration.EnchantedMobChanceMultiplier;
	return probabiltiy;
}

float DifficultyStats::GetEnchantedMobObjectiveWaveProbability() const {
	constexpr float waveMultipler = 1 / 6.f; //Assuming 1 tile spawn == 6 waves.
	return difficulty.anyEndlessStruggle() ? GetEnchantedMobProbability() * waveMultipler : 0.f;
}

float DifficultyStats::GetEnchantedMobPowerFraction() const {
	return FMath::Clamp(calculateMobEnchantmentsFraction(difficulty) + 
		FloatRange(0.0f, 0.15f).lerp(difficulty.extraChallengeFraction()), 0.0f, 1.0f) *
		endlessStruggleConfiguration.EnchantedMobPowerMultiplier;
}

float DifficultyStats::GetEnchantedMobGroupDropChance() const {
	constexpr float baseEnchantedMobGroupDropChance = 0.25f;
	return baseEnchantedMobGroupDropChance / (0.7f + 0.3f * endlessStruggleConfiguration.EnchantedMobChanceMultiplier) / (0.9f + 0.1f * endlessStruggleConfiguration.EnchantedMobPowerMultiplier);
}

float DifficultyStats::GetPlayerReceiveHealingMultiplier() const {
	return endlessStruggleConfiguration.PlayerReceiveHealingMultiplier;
}

float DifficultyStats::GetShopPriceMultiplier() const {
	//Clamp shop prices to start at easy, Highest prices at end of insane
	const float fraction = difficulty.globalThreatFractionClampedBetweenDifficulties(FDifficulty(difficultyquery::First, threatquery::First), FDifficulty(difficultyquery::Last, threatquery::Last));
	return FMath::Lerp(1.0f, 2.0f, fraction) * endlessStruggleConfiguration.ShopPriceMultiplier;
}

float DifficultyStats::GetEmeraldDropMultiplier() const {
	return FMath::Lerp(1.0f, 2.0f, difficulty.globalThreatFraction());
}

float DifficultyStats::GetXpMultiplier() const {
	const float linearMultiplier = FMath::Lerp(0.75f, 5.4f, difficulty.globalThreatFraction());
	return linearMultiplier * endlessStruggleConfiguration.XPMultiplier;
}

FloatRange DifficultyStats::GetDropItemPowerRange() const {
	return dropItemPower + extraDropItemPowerBonus;
}

FloatRange DifficultyStats::GetDisplayDropItemPowerRange() const {
	auto range = GetDropItemPowerRange();
	return { range.min(), range.max() + rareItemPowerBoost };
}

FloatRange DifficultyStats::GetRewardItemPowerRange() const {
	return rewardItemPower + extraRewardItemPowerBonus;
}


FloatRange DifficultyStats::GetRaidCaptainRewardItemPowerRange() const
{
	return raidCaptainChestItemPower + extraRewardItemPowerBonus;
}

FloatRange DifficultyStats::GetCombinedItemPowerRange() const {
	return mergeRanges(GetDropItemPowerRange(), GetRewardItemPowerRange());
}

int DifficultyStats::GetLootQualityBoost() const {
	return endlessStruggleConfiguration.LootQualityBoost;
}

void DifficultyStats::SetExtraDropItemPowerBonus(float bonus) {
	extraDropItemPowerBonus = bonus;
}

void DifficultyStats::SetExtraRewardItemPowerBonus(float bonus) {
	extraRewardItemPowerBonus = bonus;
}

bool DifficultyStats::ChangeNumberOfPlayers(unsigned int numPlayers) {
	if (numberOfPlayers != numPlayers) {
		numberOfPlayers = numPlayers;
		return true;
	}
	return false;
}

int DifficultyStats::GetNumberOfPlayers()
{
	return numberOfPlayers;
}

int DifficultyStats::GetCappedNumberOfPlayers() const {
	return FMath::Max(1, numberOfPlayers);
}




float game::DifficultyStats::GetScaledMultiplierAdjustedToNumberOfPlayers(float difficultyOffsetScaling, float perExtraPlayerScaling, float minMultiplier) const {
	//Contains extra difficulty when players are above 1.
	const float playersAboveOne = (GetCappedNumberOfPlayers() - 1);
	const float scaledPlayersAboveOneDifficulty = 1.0f + playersAboveOne * perExtraPlayerScaling;
	
	//Contains a scaled version of the combined difficulty
	const float scaledCombinedDifficulty = 1.0f + difficultyMultiplierOffset * difficultyOffsetScaling;
	
	//Scaled value starts at 1.
	const float totalScaledDifficulty = (scaledPlayersAboveOneDifficulty * scaledCombinedDifficulty);
	
	//The max below handles the case where aggression is high, and difficultyMultiplierOffset is negative
	//(difficultyMultiplierOffset can be negative because we want to balance for easy being 0, but the
	//"Free" difficulty should be easier than that)
	return FMath::Max(totalScaledDifficulty, minMultiplier);
}


const game::DifficultyStats CachedDifficultyStats::DefaultStats{ { difficultyquery::Default, threatquery::Default } };

const DifficultyStats* CachedDifficultyStats::operator->() const {
	if (mCached) {
		return mCached;
	}
	if (GWorld) {
		if (auto game = actorquery::getGame(GWorld)) {
			mCached = &game->settings().difficultyStats;
			return mCached;
		}
	}
	return &DefaultStats;
}

}
