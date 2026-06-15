#include "Dungeons.h"
#include "CauldronFormations.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"

namespace bt { namespace formation {	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeCauldronPoison(const TSubclassOf<AActor>& poisonClass) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			add(
				0s,
				EntityType::Undefined,
				poisonClass,
				FTransform { state.owner->GetActorLocation() },
				true,
				false
			);
		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeCauldronSpawn(int count, float radiusMin, float radiusMax) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			static Random rnd;

			const auto ownerLocation = state.owner->GetActorLocation();

			for (auto i = 0; i < count; i++) {
				const auto radius = rnd.nextFloat(radiusMin, radiusMax);
				const auto location = ownerLocation + (FVector { radius, 0.f, 0.f }).RotateAngleAxis(i * 360.f / count, FVector::UpVector);
				
				const auto entityType = (std::array<EntityType, 4> {
					EntityType::Zombie,
					EntityType::Husk,
					EntityType::ChickenJockey,
					EntityType::Vindicator
				})[rnd.nextInt(4)];

				add(
					i * .2s,
					entityType,
					nullptr,
					FTransform { location },
					true,
					false
				);
			}
		};
	}
}}