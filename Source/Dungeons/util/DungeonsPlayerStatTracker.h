#pragma once

#include <vector>

#include "Map.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/item/ItemTypeDefs.h"

class AItemInstance;

class DungeonsPlayerStatTracker {
public:
	template <typename T>
	using Counts = TMap<T, unsigned int>;

	enum class ScalarTrackingTypes : uint8_t {
		DamageDealt = 0,
		DamageTaken,
		Deaths,
		HealingTaken,
		HealingDone,
		QuestsCompleted,
		ChestOpened,
		AttacksMade,
		MostDamageDeltInSingleBlow,
		MostEnemiesHitInSingleBlow,
		ProjectileFired,
		ProjectilesHit,
		ScalarTrackingTypesSize,
	};

	static const char* toString(ScalarTrackingTypes);

	DungeonsPlayerStatTracker(): ScalarValues(enum_cast(ScalarTrackingTypes::ScalarTrackingTypesSize)) {}
	DungeonsPlayerStatTracker(DungeonsPlayerStatTracker&& other) : MobsKilled(std::move(other.MobsKilled)), ItemsUsed(std::move(other.ItemsUsed)), ScalarValues(std::move(other.ScalarValues)) {}
	DungeonsPlayerStatTracker(const DungeonsPlayerStatTracker&) = delete;
	DungeonsPlayerStatTracker& operator=(const DungeonsPlayerStatTracker&) = delete;
	~DungeonsPlayerStatTracker() {}

	void TrackMobKilled(EntityType);
	void TrackItemUsed(const FItemId&);
	void TrackBlockTypeWalkedOn(EMaterialTypeEnum);
	void TrackScalar(ScalarTrackingTypes, double value);

	unsigned int GetTotalKilledMobs() const { return TotalMobsKilled; }
	const Counts<EntityType>& GetKilledMobs() const { return MobsKilled; }
	const Counts<FItemId>& GetUsedItems() const { return ItemsUsed; }
	const Counts<EMaterialTypeEnum>& GetBlockTypesWalkedOn() const { return BlockTypesWalkedOn; }
	const std::vector<double>& GetScalarValues() const { return ScalarValues; }

protected:
	unsigned int TotalMobsKilled = 0;
	std::vector<double> ScalarValues;

private:
	Counts<EntityType> MobsKilled;
	Counts<FItemId> ItemsUsed;
	Counts<EMaterialTypeEnum> BlockTypesWalkedOn;

protected:
	static double operation(ScalarTrackingTypes type, double a, double b) {
		switch (type)
		{
		case ScalarTrackingTypes::MostDamageDeltInSingleBlow:
		case ScalarTrackingTypes::MostEnemiesHitInSingleBlow:
			return a > b ? a : b;
		default:
			return a + b;
		}
	}
};