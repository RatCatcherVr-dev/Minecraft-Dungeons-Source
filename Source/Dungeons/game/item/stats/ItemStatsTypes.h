#pragma once
#include "ItemStatsTypes.generated.h"

UENUM(BlueprintType)
enum class EItemStats : uint8 {
	Invalid,
	AverageDamage,
	AttackSpeed,
	QuiverAmmo,
	AreaDamage,
	DamagePerSec,
	DamagePerQuiver,
	DamagePerVolley,
	ArrowSpeed,
	AverageArea,
	PushbackStr,
	HitSnappiness,
	RechargeSpeed,
	LowestDamage,
	HighestDamage,
	DamagePerSecond,
	ProjectilesPerAttack,	
	DamageBoost,
	LowestHealthHealed,
	HighestHealthHealed,
	StunDuration,
	BlocksPushed,
	Health,
	NUM_ESTIMATES
};
ENUM_NAME(EItemStats)