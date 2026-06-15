#include "Dungeons.h"
#include "Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/BtRandom.h"
#include "game/util/Tags.h"
#include "util/CharacterQuery.h"
#include "util/Algo.h"

namespace bt { namespace actor {

const Provider& Self() {
	static Provider provider = [](StateRef state) { return state.owner; };
	return provider;
}

const Provider& Master() {
	static Provider provider = [](StateRef state) { return state.owner->GetMaster(); };
	return provider;
}

const Provider& LastAttacker() {
	static Provider provider = [](StateRef state) -> AActor* { 
		if (state.params().lastDamaged) {
			return state.params().lastDamaged->actorSource.Get();
		}

		return nullptr;
	};
	return provider;
}

const Provider& LastWarning() {
	static Provider provider = [](StateRef state) -> AActor* {
		if (state.params().lastWarning) {
			return state.params().lastWarning->actorSource.Get();
		}

		return nullptr;
	};
	return provider;
}

Provider ClosestCharacter(float maxDistance/*= 20000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float closestDistanceSquared;
		return actorquery::getClosestActor(state.owner, maxDistance, ABaseCharacter::StaticClass(), closestDistanceSquared, false, filter);
	};
}

Provider FurthestCharacter(float maxDistance/*= 20000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float furthestDistanceSquared;
		return actorquery::getFurthestActor(state.owner, maxDistance, ABaseCharacter::StaticClass(), furthestDistanceSquared, false, filter);
	};
}

Provider ClosestPlayer(float maxDistance/*= 20000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float closestDistanceSquared;
		return actorquery::getClosestPlayer(state.owner, maxDistance, closestDistanceSquared, filter);
	};
}

Provider FurthestPlayer(float maxDistance/*= 20000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float furthestDistanceSquared;
		return actorquery::getFurthestPlayer(state.owner, maxDistance, furthestDistanceSquared, filter);
	};
}

Provider ClosestMob(float maxDistance/*= 3000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float closestDistanceSquared;
		return actorquery::getClosestMob(state.owner, maxDistance, closestDistanceSquared, filter);
	};
}

Provider FurthestMob(float maxDistance/*= 3000.0f*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [maxDistance, filter](StateRef state) {
		float furthestDistanceSquared;
		return actorquery::getFurthestMob(state.owner, maxDistance, furthestDistanceSquared, filter);
	};
}

Provider ClosestEnemy(float maxDistance/*= 3000.0f*/, bool bAllowSameType/*= true*/) {
	return [&, maxDistance, bAllowSameType](StateRef state) {
		return ClosestEnemyActor(state.owner, maxDistance, bAllowSameType);
	};
}

Provider ClosestEnemyCanIgnoreFilter(float maxDistance/*= 3000.0f*/, bool bAllowSameType/*= true*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {
	return [&, maxDistance, bAllowSameType, filter](StateRef state) {
		auto found = ClosestEnemyActor(state.owner, maxDistance, bAllowSameType, filter);
		if (!found)
		{
			found = ClosestEnemyActor(state.owner, maxDistance, bAllowSameType);
		}
		return found;
	};
}

AActor* ClosestEnemyActor(AMobCharacter* source, float maxDistance/*= 3000.0f*/, bool bAllowSameType/*= true*/, std::function<bool(AActor*)> filter/* = [](AActor*) { return true; } */) {

	const auto owner = source;

	if (owner->IsFriendlyTowardsPlayers()) {
		const auto IsEnemy = [&, filter](AActor* mob) {
			return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) && Cast<AMobCharacter>(mob)->IsTargetable() && filter(mob);
		};

		float _dummySquared;
		return actorquery::getClosestMob(owner, maxDistance, _dummySquared, IsEnemy);
	}
	else {
		static const auto isAttractive = [&, filter](AActor* mob) {
			return mob->ActorHasTag(tags::isAttractive) && filter(mob);
		};

		float closestAttractiveDistanceSquared;
		const auto attractive = actorquery::getClosestPlayer(owner, maxDistance, closestAttractiveDistanceSquared, isAttractive);

		if (attractive != nullptr && closestAttractiveDistanceSquared < (400.f*400.f)) {
			return attractive;
		}

		// ---
		float closestPlayerDistanceSquared;
		const auto player = actorquery::getClosestPlayer(owner, maxDistance, closestPlayerDistanceSquared, filter);

		if (player != nullptr && closestPlayerDistanceSquared < (400.f*400.f)) {
			return player;
		}

		// ---
		const auto IsEnemy = [&, filter](AActor* mob) {
			return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) &&
				Cast<AMobCharacter>(mob)->IsTargetable() &&
				!mob->ActorHasTag(tags::pristine) &&
				(bAllowSameType || Cast<AMobCharacter>(mob)->EntityType != owner->EntityType) &&
				filter(mob);
		};

		float closestFriendlyDistanceSquared;
		const auto mob = actorquery::getClosestMob(owner, maxDistance, closestFriendlyDistanceSquared, IsEnemy);

		// ---
		const auto playerOrAttractive = closestPlayerDistanceSquared * 1.5f < closestAttractiveDistanceSquared ? player : attractive;

		if (playerOrAttractive != nullptr && mob != nullptr) {
			return closestFriendlyDistanceSquared < closestPlayerDistanceSquared ? mob : playerOrAttractive;
		}
		else if (playerOrAttractive != nullptr) {
			return playerOrAttractive;
		}
		else {
			return mob;
		}
	}
}

Provider FurthestEnemy(float maxDistance /*= 3000.0f*/) {
	return [&, maxDistance](StateRef state) {
		const auto owner = state.owner;

		if (owner->IsFriendlyTowardsPlayers()) {
			const auto IsEnemy = [&](AActor* mob) {
				return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) && Cast<AMobCharacter>(mob)->IsTargetable();
			};

			float _dummySquared;
			return actorquery::getFurthestMob(owner, maxDistance, _dummySquared, IsEnemy);
		}
		else {
			static const auto isAttractive = [&](AActor* mob) {
				return mob->ActorHasTag(tags::isAttractive);
			};

			float furthestAttractiveDistanceSquared;
			const auto attractive = actorquery::getFurthestPlayer(owner, maxDistance, furthestAttractiveDistanceSquared, isAttractive);

			// ---
			float furthestPlayerDistanceSquared;
			const auto player = actorquery::getFurthestPlayer(owner, maxDistance, furthestPlayerDistanceSquared);

			// ---
			const auto IsEnemy = [&](AActor* mob) {
				return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) &&
					Cast<AMobCharacter>(mob)->IsTargetable() &&
					!mob->ActorHasTag(tags::pristine);
			};

			float furthestFriendlyDistanceSquared;
			const auto mob = actorquery::getFurthestMob(owner, maxDistance, furthestFriendlyDistanceSquared, IsEnemy);

			// ---
			const auto playerOrAttractive = furthestPlayerDistanceSquared > furthestAttractiveDistanceSquared * 1.5f ? player : attractive;

			if (playerOrAttractive != nullptr && mob != nullptr) {
				return furthestFriendlyDistanceSquared > furthestPlayerDistanceSquared ? mob : playerOrAttractive;
			}
			else if (playerOrAttractive != nullptr) {
				return playerOrAttractive;
			}
			else {
				return mob;
			}
		}
	};
}

Provider ClosestEnemyToLocation(const locator::Provider& originProvider, float maxDistance) {
	return [originProvider, maxDistance](StateRef state) {	
		const auto owner = state.owner;

		const auto origin = originProvider(state).GetLocationLike();
		const auto location = origin.Get(owner->GetActorLocation());
		
		if (owner->IsFriendlyTowardsPlayers()) {
			const auto IsEnemy = [&](AActor* mob) {
				return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) && Cast<AMobCharacter>(mob)->IsTargetable();
			};

			float _dummySquared;
			return actorquery::getClosestToLocation<AMobCharacter>(location, maxDistance, _dummySquared, &state.world(), IsEnemy);
		}
		else {
			static const auto isAttractive = [&](AActor* mob) {
				return mob->ActorHasTag(tags::isAttractive);
			};

			float closestAttractiveDistanceSquared;
			const auto attractive = actorquery::getClosestToLocation<APlayerCharacter>(location, maxDistance, closestAttractiveDistanceSquared, &state.world(), isAttractive);

			if (attractive != nullptr && closestAttractiveDistanceSquared < (400.f*400.f)) {
				return attractive;
			}

			// ---
			float closestPlayerDistanceSquared;
			const auto player = actorquery::getClosestToLocation<APlayerCharacter>(location, maxDistance, closestPlayerDistanceSquared, &state.world());

			if (player != nullptr && closestPlayerDistanceSquared < (400.f*400.f)) {
				return player;
			}

			// ---
			const auto IsEnemy = [=](AActor* mob) {
				return owner->IsHostileTowards(Cast<AMobCharacter>(mob)) &&
					Cast<AMobCharacter>(mob)->IsTargetable() &&
					!mob->ActorHasTag(tags::pristine) &&
					(Cast<AMobCharacter>(mob)->EntityType != owner->EntityType);
			};

			float closestFriendlyDistanceSquared = 0.f;
			const auto mob = actorquery::getClosestToLocation<AMobCharacter>(location, maxDistance, closestFriendlyDistanceSquared, &state.world(), IsEnemy);

			// ---
			const auto playerOrAttractive = closestPlayerDistanceSquared * 1.5f < closestAttractiveDistanceSquared ? player : attractive;

			if (playerOrAttractive != nullptr && mob != nullptr) {
				return closestFriendlyDistanceSquared < closestPlayerDistanceSquared ? mob : playerOrAttractive;
			}
			else if (playerOrAttractive != nullptr) {
				return playerOrAttractive;
			}
			else {
				return mob;
			}
		}
	};
}

Provider RandomPlayer(const locator::Provider& originProvider, float maxDistance) {
	return [=](StateRef state) {
		TArray<APlayerCharacter*> playerArray;

		float distanceSquared = maxDistance * maxDistance;

		if (const auto origin = originProvider(state).GetLocationLike()) {
			for (auto&& candidate : InstanceTracker<APlayerCharacter>::GetList(&state.world())) {
				if (FVector::DistSquared(origin.GetValue(), candidate->GetActorLocation()) < distanceSquared) {
					playerArray.Add(candidate);
				}
			}			
		}
		return playerArray[random::randomInt(0, playerArray.Num())(state)];
	};
}

bt::Provider<int> CountInRange(const locator::Provider& originProvider, float maxDistance, const std::function<bool(AActor*)>& filter) {
	return [=](StateRef state) {
		if (const auto origin = originProvider(state).GetLocationLike()) {			
			const auto filterAndInRange = [=](AActor* candidate) {
				return filter(candidate) && FVector::PointsAreNear(origin.GetValue(), candidate->GetActorLocation(), maxDistance);
			};

			return actorquery::countActors<ABaseCharacter>(&state.world(), filterAndInRange);
		}

		return 0;
	};
}

AActor* Target::operator()(StateRef state) const {
	return state.params().target.actor.Get();
}

void Target::operator()(StateRef state, AActor* actor) const {
	if (actor && !actor->IsValidLowLevel()) {
		actor = nullptr;
	}
	state.params().target = FLocator(actor);
}

AActor* TargetSecondary::operator()(StateRef state) const {
	return state.params().targetSecondary.actor.Get();
}

void TargetSecondary::operator()(StateRef state, AActor* actor) const {
	if (actor && !actor->IsValidLowLevel()) {
		actor = nullptr;
	}
	state.params().targetSecondary = FLocator(actor);
}

MultiProvider toMulti(const Provider& single) {
	return [=](StateRef state) -> TArray<AActor*> {
		return { single(state) };
	};
}

MultiProvider ClosestMobs(
	float maxDistance/* = 3000.0f*/,
	int count/* = 1*/,
	std::function<bool(AMobCharacter*)> filter/* = [](AMobCharacter*) { return true; }*/
) {
	return [maxDistance, filter](StateRef state) {
		return algo::map_cast<TArray<AActor*>>(actorquery::getNearbyActors<AMobCharacter>(state.owner, maxDistance, filter));		
	};
}


}}
