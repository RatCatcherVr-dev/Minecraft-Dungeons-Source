#pragma once

#include "Difficulty.h"
#include "EndlessStruggle.h"
#include "util/FloatRange.h"

namespace game {

struct DifficultyStats {

	enum class EPowerRangeBonus {
		ExtraChallengeBonus,
		MidGameAffectorsBonus,
	};

	DifficultyStats(FDifficulty);
	DifficultyStats& operator=(const DifficultyStats&) = default;

	const FDifficulty& GetDifficulty() const;
	
	float GetMobMaxHealthMultiplier() const;
	float GetMobPerformHealingMultiplier() const;
	float GetMobMaxEnduranceMultiplier() const;
	float GetMobRecoverEnduranceMultiplier() const;
	float GetMobDamageMultiplier() const;
	float GetSpecialMobMaxHealthMultiplier() const;
	float GetMobSpeedMultiplier() const; // D11.DB
	float GetMobResurrectionChance() const; // D11.DB
	float GetMobDeflectChance() const; // D11.DB
	float GetMobPushbackMultiplier() const; // D11.KS

	float GetMobStunDurationMultiplier() const;
	float GetPlayerCountMobMultiplier() const;
	float GetEndlessStruggleMobMultiplier() const;
	float GetMobCountMultiplier() const;

	float GetEnchantedMobProbability() const;
	float GetEnchantedMobObjectiveWaveProbability() const;
	float GetEnchantedMobPowerFraction() const;
	float GetEnchantedMobGroupDropChance() const;

	float GetPlayerReceiveHealingMultiplier() const;

	float GetXpMultiplier() const;
	float GetShopPriceMultiplier() const;	
	float GetEmeraldDropMultiplier() const;
	
	FloatRange GetDropItemPowerRange() const;
	FloatRange GetDisplayDropItemPowerRange() const;
	FloatRange GetRewardItemPowerRange() const;
	FloatRange GetRaidCaptainRewardItemPowerRange() const;
	FloatRange GetCombinedItemPowerRange() const;
	float GetRareItemPowerBoost() const { return rareItemPowerBoost; }
	int GetLootQualityBoost() const;

	void SetExtraDropItemPowerBonus(float);
	void SetExtraRewardItemPowerBonus(float);

	bool ChangeNumberOfPlayers(unsigned int);
	int GetNumberOfPlayers();

	const FEndlessStruggleConfiguration& GetEndlessStruggleConfiguration() { return endlessStruggleConfiguration; };

private:
	float difficultyMultiplierOffset;

	int GetCappedNumberOfPlayers() const;
	float GetScaledMultiplierAdjustedToNumberOfPlayers(float difficultyOffsetScaling = 1.0f, float perExtraPlayerScaling = 0.333f, float minMultiplier = 0.1f) const;

	FDifficulty difficulty;
	FEndlessStruggleConfiguration endlessStruggleConfiguration;
	int numberOfPlayers = 1;

	FloatRange dropItemPower;
	FloatRange rewardItemPower;
	FloatRange raidCaptainChestItemPower;
	float rareItemPowerBoost;

	float extraDropItemPowerBonus;
	float extraRewardItemPowerBonus;
};

// Used to look up the DifficultyStats in a simpler manner
// @todo: figure out if we should put the DifficultyStats
//        somewhere else, and how to access them
class CachedDifficultyStats {
public:
	const DifficultyStats* operator->() const;
	static const DifficultyStats& GetDefault() { return DefaultStats; }

private:
	static const DifficultyStats DefaultStats;
	mutable const DifficultyStats* mCached;
};

}

