#include "Dungeons.h"
#include "DungeonsGameStatTracker.h"
#include "game/item/instance/AItemInstance.h"
#include "game/util/ValueFormat.h"

const char* DungeonsGameStatTracker::toString(EGameTrackingTypes trackingType) {	

	switch (trackingType) {
	case EGameTrackingTypes::ChestsOpened:
		return "chests_opened";
	case EGameTrackingTypes::ChestsSpawned:
		return "chests_spawned";
	case EGameTrackingTypes::DamageDealt:
		return "damage_dealt";
	case EGameTrackingTypes::EmeraldsFound:
		return "emeralds_found";
	case EGameTrackingTypes::EmeraldsSpawned:
		return "emeralds_spawned";
	case EGameTrackingTypes::GearPickedUp:
		return "gear_picked_up";
	case EGameTrackingTypes::MobsSpawned:
		return "mobs_spawned";
	case EGameTrackingTypes::MobsKilled:
		return "mobs_killed";
	case EGameTrackingTypes::PlayerDamageTaken:
		return "player_damage_taken";
	case EGameTrackingTypes::PlayerDeaths:
		return "player_deaths";
	case EGameTrackingTypes::PlayerHealingDone:
		return "player_healing_done";
	case EGameTrackingTypes::PlayerProjectileFired:
		return "player_projectile_fired";
	case EGameTrackingTypes::PlayerProjectileHit:
		return "player_projectile_hit";
	case EGameTrackingTypes::Size:
		return "scalar_tracking_type_size";
	default:
		return "";
	}
	//C4715 warning/error fix
	return nullptr;
}

#define LOCTEXT_NAMESPACE "GameStats"
const FText& DungeonsGameStatTracker::TextPercentage() {
	static FText s_textPercentage = LOCTEXT("text_percentage", "{percentage}%");
	return s_textPercentage;
}

int DungeonsGameStatTracker::ToTrackingTypeDisplayCount(EGameTrackingTypes type, double count)
{
	return IsTrackingTypeHealth(type) ? valueformat::healthAsDisplayAmount((float)count) : (int)count;
}

const FText DungeonsGameStatTracker::toText(EGameTrackingTypes trackingType) {

	switch (trackingType) {
	case EGameTrackingTypes::ChestsOpened:
		return LOCTEXT("chests_opened", "Chests Opened");
	case EGameTrackingTypes::ChestsSpawned:
		return LOCTEXT("chests_spawned", "Chests in Level");
	case EGameTrackingTypes::DamageDealt:
		return LOCTEXT("damage_dealt", "Damage Dealt");
	case EGameTrackingTypes::EmeraldsFound:
		return LOCTEXT("emeralds_found", "Emeralds Found");
	case EGameTrackingTypes::EmeraldsSpawned:
		return LOCTEXT("emeralds_spawned", "Emeralds in Level");
	case EGameTrackingTypes::GearPickedUp:
		return LOCTEXT("gear_picked_up", "Gear Found");
	case EGameTrackingTypes::MobsSpawned:
		return LOCTEXT("mobs_spawned", "Mobs Spawned");
	case EGameTrackingTypes::MobsKilled:
		return LOCTEXT("mobs_killed", "Mobs Defeated");
	case EGameTrackingTypes::PlayerDamageTaken:
		return LOCTEXT("player_damage_taken", "Damage Taken");
	case EGameTrackingTypes::PlayerDeaths:
		return LOCTEXT("player_deaths", "Players Downed");
	case EGameTrackingTypes::PlayerHealingDone:
		return LOCTEXT("player_healing_done", "Healing done");
	case EGameTrackingTypes::PlayerProjectileFired:
		return LOCTEXT("player_projectile_fired", "Projectiles Fired");
	case EGameTrackingTypes::PlayerProjectileHit:
		return LOCTEXT("player_projectile_hit", "Projectiles Hit");
	case EGameTrackingTypes::Size:
		return LOCTEXT("scalar_tracking_type_size", "SIZE __");
	default:
		return FText();
	}

	return FText();

}

#undef LOCTEXT_NAMESPACE

bool DungeonsGameStatTracker::CheckIsTracked(EGameTrackingTypes type) const
{
	// if has linked pair, both must be tracked!
	auto it = TrackingPairs.find(type);
	if (it != TrackingPairs.end())
	{
		EGameTrackingTypes ptype = it->second;
		return GameCounters.find(type) != GameCounters.end() && GameCounters.find(ptype) != GameCounters.end();
	}
	else
	{
		return GameCounters.find(type) != GameCounters.end();
	}
}

std::vector<DungeonsGameStatTracker::DungeonsGameStatPair> DungeonsGameStatTracker::GetTrackedStats() //const
{
	std::vector<DungeonsGameStatPair> ret;
	for (auto p : TrackingPairs)
	{
		DungeonsGameStatTracker::DungeonsGameStatPair pair_out;
		pair_out.Type = p.first,
		pair_out.Count = GameCounters[p.first];
		if (p.second < EGameTrackingTypes::Size)
		{
			pair_out.TypeTotal = p.second;
			pair_out.CountTotal = GameCounters[p.second];
		}
		ret.emplace_back(std::move(pair_out));

	} 

	return ret;
}

double DungeonsGameStatTracker::GetTrackingTypeCount(EGameTrackingTypes type)
{	
	return GameCounters[type];
}

void DungeonsGameStatTracker::TrackMobKilled(EntityType mobType)
{	
	TrackScalar(EGameTrackingTypes::MobsKilled, 1);
}


void DungeonsGameStatTracker::TrackMobSpawned(EntityType mobType) 
{
	TrackScalar(EGameTrackingTypes::MobsSpawned, 1);
}

void DungeonsGameStatTracker::TrackStatEvent(EGameTrackingTypes type) 
{
	TrackScalar(type, 1);
}

void DungeonsGameStatTracker::TrackScalar(EGameTrackingTypes type, double value) {
	GameCounters[type] = DungeonsGameStatTracker::operation(type, GameCounters[type], value);
}







int DungeonsGameStatTracker::DungeonsGameStatPair::GetDisplayCount() const
{
	return ToTrackingTypeDisplayCount(Type, Count);
}

int DungeonsGameStatTracker::DungeonsGameStatPair::GetDisplayTotal() const
{
	return ToTrackingTypeDisplayCount(TypeTotal, CountTotal);
}

FText DungeonsGameStatTracker::DungeonsGameStatPair::GetDisplayPercentageText() const
{
	const float percentage = CountTotal > 0 ? Count / CountTotal : 0;
	return valueformat::asPercentageText(percentage);
}
