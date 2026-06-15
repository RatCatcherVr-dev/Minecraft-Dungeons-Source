#pragma once
#include "AITypes.h"
#include "game/ai/bt/BtTime.h"
#include "game/Locator.h"
#include <GameplayTagContainer.h>

struct FAttackParams {
	int all = 0;
	int melee = 0;
	int ranged = 0;
};

struct FDamagedParams {
	TWeakObjectPtr<AActor> actorSource;
	FVector location;
	bt::TimeStamp timeStamp;
	FGameplayTagContainer gameplayTagContainer;
};

struct FWarnedParams {
	TWeakObjectPtr<AActor> actorSource;	
	bt::TimeStamp timeStamp;
};

struct FMobParams {
	FLocator target;
	FLocator targetSecondary;
	FVector anchor = FAISystem::InvalidLocation;
	FVector startPos = FAISystem::InvalidLocation;

	FAttackParams successfulAttacks;
	FAttackParams totalAttacks;

	bt::TimeStamp spawnTime;
	bt::TimeStamp teleportTime;
	bt::TimeStamp lastAttackTime;
	bt::TimeStamp lastDefenseTime;
	bt::TimeStamp canWarnTime;

	TOptional<FDamagedParams> lastDamaged;
	TOptional<FWarnedParams> lastWarning;
};
