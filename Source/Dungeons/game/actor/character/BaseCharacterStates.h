#pragma once

#include "BaseCharacterStates.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8 {
	Idle,
	Walking,
	Running,
	Flying,
	Staggered,
	Dashing,
	Rolling, 
	Usingitem
};


UENUM(BlueprintType)
enum class ECharacterWorldState : uint8 {
	InWorld,
	Disappeared,
	DisappearedWithCollision
};

UENUM(BlueprintType)
enum class EAttackState : uint8 {
	None,
	Aiming,
	Melee,
	Ranged,
	Magic,
	Targeting,
};
//
//UENUM(BlueprintType)
//enum class ECharacterStateAffected : uint8 {
//	None,
//	Weakened,
//	Powered,
//	RangedPowered, 
//	Speedup,
//	Speeddown,
//	DefenceUp,
//	BootsOfSwiftnessSpeedBoost,
//	Onfire,
//	GhostForm, 
//	DeathCapBerserk, 
//	Stunned
//};
UENUM(BlueprintType)
enum class EMobState : uint8 {
	Idle,
	Combat,
	Staggered,
	Fleeing,
	Custom1
};
