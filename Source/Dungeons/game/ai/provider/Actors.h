#pragma once
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"
#include "game/util/ActorQuery.h"

namespace bt { namespace actor {

using Provider = bt::Provider<AActor*>;
using MultiProvider = bt::Provider<TArray<AActor*>>;

const Provider& Self();
const Provider& Master();
const Provider& LastAttacker();
const Provider& LastWarning();

template <typename T> Provider ClosestActor(float maxDistance = 20000.0f,
	std::function<bool (AActor*)> filter = [](AActor*) { return true; }
) {
	return [maxDistance, filter](StateRef state) {
		float closestDistanceSquared;
		return actorquery::getClosestActor(state.owner, maxDistance, T::StaticClass(), closestDistanceSquared, false, filter);
	};
};

template <typename T> Provider FurthestActor(float maxDistance = 20000.0f,
	std::function<bool (AActor*)> filter = [](AActor*) { return true; }
) {
	return [maxDistance, filter](StateRef state) {
		float furthestDistanceSquared;
		return actorquery::getFurthestActor(state.owner, maxDistance, T::StaticClass(), furthestDistanceSquared, false, filter);
	};
};

Provider ClosestCharacter(
	float maxDistance = 20000.0f,
	std::function<bool (AActor*)> = [](AActor*) { return true; }
);

Provider FurthestCharacter(
	float maxDistance = 20000.0f,
	std::function<bool(AActor*)> = [](AActor*) { return true; }
);

Provider ClosestPlayer(
	float maxDistance = 20000.0f,
	std::function<bool (AActor*)> = [](AActor*) { return true; }
);

Provider FurthestPlayer(
	float maxDistance = 20000.0f,
	std::function<bool (AActor*)> = [](AActor*) { return true; }
);

Provider ClosestMob(
	float maxDistance = 3000.0f,
	std::function<bool (AActor*)> = [](AActor*) { return true; }
);

Provider FurthestMob(
	float maxDistance = 3000.0f,
	std::function<bool (AActor*)> = [](AActor*) { return true; }
);

Provider ClosestEnemy(float maxDistance = 3000.0f, bool bAllowSameType = true);

Provider ClosestEnemyCanIgnoreFilter(
	float maxDistance = 3000.0f, bool bAllowSameType = true,
	std::function<bool(AActor*)> = [](AActor*) { return true; }
);

AActor* ClosestEnemyActor(
	AMobCharacter* source,
	float maxDistance = 3000.0f, bool bAllowSameType = true,
	std::function<bool(AActor*)> = [](AActor*) { return true; }
);

Provider FurthestEnemy(float maxDistance = 3000.0f);

Provider ClosestEnemyToLocation (
	const locator::Provider& originProvider,
	float maxDistance = 20000.0f
);

Provider RandomPlayer(const locator::Provider& originProvider, float maxDistance);

bt::Provider<int> CountInRange(
	const locator::Provider&,
	float,
	const std::function<bool(AActor*)>&
);

struct Target {
	AActor* operator()(StateRef) const;
	void operator()(StateRef, AActor*) const;
};

struct TargetSecondary {
	AActor* operator()(StateRef) const;
	void operator()(StateRef, AActor*) const;
};

MultiProvider toMulti(const Provider&);

MultiProvider ClosestMobs(
	float maxDistance = 3000.0f,
	int count = 1,
	std::function<bool(AMobCharacter*)> = [](AMobCharacter*) { return true; }
);


}}
