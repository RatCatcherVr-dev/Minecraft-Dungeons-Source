#pragma once

#include "DungeonsPlayerStatTracker.h"

UENUM(BlueprintType)
enum class EGameTrackingTypes : uint8 {
	ChestsOpened = 0,
	ChestsSpawned,
	DamageDealt,
	EmeraldsFound,
	EmeraldsSpawned,
	GearSpawned,
	GearPickedUp,
	MobsSpawned,
	MobsKilled,
	PlayerDamageTaken,
	PlayerDeaths,
	PlayerHealingDone,
	PlayerProjectileFired,
	PlayerProjectileHit,
	Size
};

class AItemInstance;

class DungeonsGameStatTracker {
public:
	struct DungeonsGameStatPair {
		EGameTrackingTypes Type;
		EGameTrackingTypes TypeTotal = EGameTrackingTypes::Size;
		double Count;
		double CountTotal = 0;

		int GetDisplayCount() const;
		int GetDisplayTotal() const;		
		FText GetDisplayPercentageText() const;
		// desc?
	};
	
	static const FText& TextPercentage();

	static int ToTrackingTypeDisplayCount(EGameTrackingTypes type, double count);

	static bool IsTrackingTypeHealth(EGameTrackingTypes type)
	{
		switch (type)
		{
		case EGameTrackingTypes::DamageDealt:
		case EGameTrackingTypes::PlayerDamageTaken:
		case EGameTrackingTypes::PlayerHealingDone:
			return true;			
		default:
			return false;
		}		
	}
	

private:
	// since DungeonsPlayerStatTracker::ScalarTrackingTypes uses enum-indices to index
	// ScalarTypes vector, this cannot easily be extended -just place non-reoccurring stat counters
	// in their own container:
	// the overhead from using 'map' over 'vector' here is negligible
	std::map<EGameTrackingTypes, double> GameCounters;

	// need to calculate fractions stats, only possible if pairs are both tracked!
	const std::map<EGameTrackingTypes, EGameTrackingTypes> TrackingPairs =
	{
		{ EGameTrackingTypes::ChestsOpened, EGameTrackingTypes::ChestsSpawned },
		{ EGameTrackingTypes::MobsKilled, EGameTrackingTypes::MobsSpawned },
		{ EGameTrackingTypes::PlayerProjectileHit, EGameTrackingTypes::PlayerProjectileFired },
		{ EGameTrackingTypes::PlayerDamageTaken, EGameTrackingTypes::Size },
		{ EGameTrackingTypes::PlayerHealingDone, EGameTrackingTypes::Size },
		{ EGameTrackingTypes::DamageDealt, EGameTrackingTypes::Size },
		{ EGameTrackingTypes::PlayerDeaths, EGameTrackingTypes::Size }
	};

public:
	static const char* toString(EGameTrackingTypes);
	static const FText toText(EGameTrackingTypes);
	
	bool CheckIsTracked(EGameTrackingTypes type) const;
	std::vector<DungeonsGameStatPair> GetTrackedStats();

	double GetTrackingTypeCount(EGameTrackingTypes type);

	void TrackMobKilled(EntityType mobType);
	void TrackMobSpawned(EntityType mobType);

	void TrackStatEvent(EGameTrackingTypes type);

	void TrackScalar(EGameTrackingTypes type, double value);

	static double operation(EGameTrackingTypes type, double a, double b) {
		return a + b;		
	}

	DungeonsGameStatTracker() {}
	DungeonsGameStatTracker(DungeonsGameStatTracker&& other)  {}
	DungeonsGameStatTracker(const DungeonsGameStatTracker&) = delete;
	DungeonsGameStatTracker& operator=(const DungeonsGameStatTracker&) = delete;

};