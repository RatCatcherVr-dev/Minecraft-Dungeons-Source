#pragma once

#include "game/actor/EffectsActor.h"
#include "game/Enchantments/mobs/MobEnchantmentTypes.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "world/phys/Vec3.h"
#include <Engine/World.h>
#include <UnrealString.h>
#include <functional>
#include <queue>

class AMobCharacter;

namespace game {  namespace objective {

class SpawnQueue {
public:
	SpawnQueue(UWorld&, mobspawn::Config, FString effectsPackName, std::function<void(AMobCharacter*)> spawnCallback);

	void tick(float time);
	void enqueue(float delay, float duration, const mobspawn::SpawnGroup&, bool isLarge, const std::vector<Vec3>& blockPos);
	bool isEmpty() const;

private:
	struct PreSpawnEntry {
		float delay;
		std::vector<Vec3> blockPos;
		bool isLarge;
	};

	struct SpawnEntry {
		float delay;
		mobspawn::SpawnGroup group;
		std::vector<Vec3> blockPos;
		bool isLarge;
	};

	void onPreSpawn(const PreSpawnEntry&);
	void onSpawn(const SpawnEntry&);

	std::queue<PreSpawnEntry> preSpawnQueue;
	std::queue<SpawnEntry> spawnQueue;

	std::function<void(AMobCharacter*)> spawnCallback;
	mobspawn::Config spawnConfig;
	UWorld& world;
	FString effectsPackName;
	TWeakObjectPtr<AEffectsActor> effectsActor;
};

}}
