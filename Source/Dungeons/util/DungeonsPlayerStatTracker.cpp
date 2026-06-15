#include "Dungeons.h"
#include "DungeonsPlayerStatTracker.h"
#include "game/item/instance/AItemInstance.h"

const char* DungeonsPlayerStatTracker::toString(ScalarTrackingTypes trackingType) {
	switch (trackingType) {
	case ScalarTrackingTypes::DamageDealt:
		return "damage_dealt";
	case ScalarTrackingTypes::DamageTaken:
		return "damage_taken";
	case ScalarTrackingTypes::Deaths:
		return "deaths";
	case ScalarTrackingTypes::HealingTaken:
		return "healing_taken";
	case ScalarTrackingTypes::HealingDone:
		return "healing_done";
	case ScalarTrackingTypes::QuestsCompleted:
		return "quests_completed";
	case ScalarTrackingTypes::ChestOpened:
		return "chest_opened";
	case ScalarTrackingTypes::AttacksMade:
		return "attacks_made";
	case ScalarTrackingTypes::MostDamageDeltInSingleBlow:
		return "most_damage_in_single_blow";
	case ScalarTrackingTypes::MostEnemiesHitInSingleBlow:
		return "most_enemies_in_single_blow";
	case ScalarTrackingTypes::ProjectileFired:
		return "projectile_fired";
	case ScalarTrackingTypes::ProjectilesHit:
		return "projectiles_hit";
	default:
		return "";
	}
	//C4715 warning/error fix
	return nullptr;
}

void DungeonsPlayerStatTracker::TrackMobKilled(EntityType mobType) {
	MobsKilled.FindOrAdd(mobType)++;
	TotalMobsKilled++;
}

void DungeonsPlayerStatTracker::TrackItemUsed(const FItemId& itemType) {
	ItemsUsed.FindOrAdd(itemType)++;
}

void DungeonsPlayerStatTracker::TrackBlockTypeWalkedOn(EMaterialTypeEnum type) {
	BlockTypesWalkedOn.FindOrAdd(type)++;
}

void DungeonsPlayerStatTracker::TrackScalar(ScalarTrackingTypes type, double value) {
	ScalarValues[enum_cast(type)] = DungeonsPlayerStatTracker::operation(type, ScalarValues[enum_cast(type)], value);
}
