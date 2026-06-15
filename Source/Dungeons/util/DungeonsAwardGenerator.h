// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonsPlayerStatTracker.h"
#include "DungeonsGameStatTracker.h"
#include <Union.h>
#include <vector>


class APlayerController;

UENUM(BlueprintType)
enum class EAwardType : uint8 {
	Invalid,
	MostEnemiesHit,
	MostDamageDoneSingleBlow,
	MostEnemiesKilled,
	MostZombiesKilled,
	MostSkeletonsKilled,
	MostSpidersKilled,
	MostDamageDone,
	BlocksWalkedOn,
	MostArrowsFired,
	MostArrowsHit,
	StrengthPotionsUsed,
	SwiftnessPotionsUsed,
	TNTUsed,
	HealthPotionsUsed,
	ItemsUsed,
	LeastDamageTaken,
	GameMobsKilled
};
ENUM_NAME(EAwardType);

/**
 * 
 */

namespace analytics
{
	struct PlayerStats;
}

namespace awards {
	struct AwardData {
		EAwardType type;
		int32 score;
		int playerNumber;

		AwardData() : type(EAwardType::Invalid), score(0), playerNumber(0) {}
		AwardData(int playerNumber, EAwardType type, double score) : type(type), score(score), playerNumber(playerNumber) {}
	};
	struct AwardResult {
		std::vector<AwardData> all;
		std::vector<AwardData> best;
	};

	static bool IsHealthDisplay(EAwardType type)
	{
		switch (type)
		{
		case EAwardType::MostDamageDoneSingleBlow:
		case EAwardType::MostDamageDone:
		case EAwardType::LeastDamageTaken:
			return true;
		default: return false;
		}
	}

	static const char* toString(EAwardType type)
	{
		switch (type)
		{
		case EAwardType::MostEnemiesHit:
			return "most_enemies_hit";
		case EAwardType::MostDamageDoneSingleBlow:
			return "most_damage_done_single_blow";
		case EAwardType::MostEnemiesKilled:
			return "most_enemies_killed";
		case EAwardType::MostZombiesKilled:
			return "most_zombies_killed";
		case EAwardType::MostSkeletonsKilled:
			return "most_skeletons_killed";
		case EAwardType::MostSpidersKilled:
			return "most_spiders_killed";
		case EAwardType::MostDamageDone:
			return "most_damage_done";
		case EAwardType::BlocksWalkedOn:
			return "blocks_walked_on";
		case EAwardType::MostArrowsFired:
			return "most_arrows_fired";
		case EAwardType::MostArrowsHit:
			return "most_arrows_hit";
		case EAwardType::StrengthPotionsUsed:
			return "strenth_potions_used";
		case EAwardType::SwiftnessPotionsUsed:
			return "swiftness_potions_used";
		case EAwardType::TNTUsed:
			return "tnt_used";
		case EAwardType::HealthPotionsUsed:
			return "health_potions_used";
		case EAwardType::ItemsUsed:
			return "items_used";
		case EAwardType::LeastDamageTaken:
			return "least_damage_taken";
		case EAwardType::GameMobsKilled:
			return "game_mobs_killed";
			//case Invalid
		default: return "";
		}
	}

	AwardResult GenerateAwards(const TMap<int, DungeonsPlayerStatTracker>& playerStats, const TMap<EAwardType, float>& threshhold, ELevelNames level = ELevelNames::creeperwoods, EGameDifficulty difficulty = EGameDifficulty::Difficulty_1);
	void GeneratePlayerStats(const DungeonsPlayerStatTracker& statTracker, analytics::PlayerStats& outStats);
}
