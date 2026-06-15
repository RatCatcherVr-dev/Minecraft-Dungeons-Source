#include "Dungeons.h"
#include "SpawnQueue.h"
#include "game/Conversion.h"
#include "game/actor/EffectsActor.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/LocationQuery.h"
#include "game/Enchantments/mobs/MobEnchantmentTypes.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/GameBP.h"

namespace game { namespace objective {

using namespace std::placeholders;

SpawnQueue::SpawnQueue(UWorld& world, mobspawn::Config config, FString effectsPackName, std::function<void(AMobCharacter*)> callback)
	: world(world)
	, effectsPackName(std::move(effectsPackName))
	, spawnCallback(std::move(callback))
	, spawnConfig(std::move(config))
{
	spawnConfig.tries = 1; // Override the tries here, since we haven't yet updated the "iterate a vector for each mob, and hope collection size matches spawn group size"
}

template <typename T, typename Apply>
static void drain(std::queue<T>& queue, float now, const Apply& apply) {
	while (!queue.empty()) {
		const auto entry = queue.front();

		if (entry.delay > now) {
			break;
		}
		queue.pop();
		apply(entry);
	}
}

void SpawnQueue::tick(float time) {
	if (!effectsActor.IsValid()) {
		effectsActor = AEffectsActor::GetInstance(&world);
	}

	const auto now = world.GetTimeSeconds();
	drain(preSpawnQueue, now, std::bind(&SpawnQueue::onPreSpawn, this, _1));
	drain(spawnQueue, now, std::bind(&SpawnQueue::onSpawn, this, _1));
}

void SpawnQueue::onPreSpawn(const PreSpawnEntry& entry) {
	if (effectsActor.IsValid()) {
		for (const auto& location : entry.blockPos) {
			const auto translation = conversion::posToUe(location);
			if (const auto ground = locationquery::findGround(world, translation)) {
				effectsActor->PreSpawnEffectsAtLocation(
					effectsPackName,
					entry.isLarge,
					{ translation.X, translation.Y, ground.GetValue() }
				);
			}
		}
	}
}

void SpawnQueue::onSpawn(const SpawnEntry& entry) {
	auto it = entry.blockPos.begin();
	const auto transformProvider = [&]() -> FTransform {
		return FTransform{
			conversion::posToUe(*it++),
		};
	};

	const bool bIsLarge = entry.isLarge;

	AGameBP* game = actorquery::getFirstActor<AGameBP>(&world);
	
	FString capturedEffectsPackName = effectsPackName;
	TWeakObjectPtr<AEffectsActor> capturedEffectsActor = effectsActor;
	std::function<void(AMobCharacter*)> capturedSpawnCallback = spawnCallback;
	
	game->RequestMobGroupSpawn(entry.group, transformProvider, spawnConfig, [](TArray<AMobCharacter*>& pMob) {}, [capturedEffectsActor, capturedEffectsPackName, capturedSpawnCallback, bIsLarge](AMobCharacter* pMob) {
	
		if (pMob && capturedEffectsActor.IsValid()) {
			capturedEffectsActor->SpawnEffectsAtLocation(capturedEffectsPackName, bIsLarge, pMob->GetActorLocation());
		}
		capturedSpawnCallback(pMob);

	});	
}

void SpawnQueue::enqueue(float time, float duration, const mobspawn::SpawnGroup& group, bool isLarge, const std::vector<Vec3>& blockPos) {
	preSpawnQueue.push({ time, blockPos, isLarge });
	spawnQueue.push({ time + duration, group, blockPos, isLarge });
}

bool SpawnQueue::isEmpty() const {
	return spawnQueue.empty();
}

}}
