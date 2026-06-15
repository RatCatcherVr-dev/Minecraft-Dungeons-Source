#include "Dungeons.h"
#include "RedstoneFormations.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"

namespace bt { namespace formation {
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeHorsemenSquare(float radiusBase, float radiusIncrement) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			static Random rnd;
			const auto angleBase = rnd.nextFloat(0.f, 360.f);

			const auto center = state.owner->GetActorLocation();

			const auto count { 4 };
			for (auto i = 0; i < count; i++) {
				const auto angle = 360.f / count * i + angleBase;
				const auto radius = radiusIncrement * i + radiusBase;
				const auto location = center + (FVector::ForwardVector * radiusBase).RotateAngleAxis(angle, FVector::UpVector);
				const FRotator rotation { 0.f, angle, 0.f };

				add(
					i * .15s,
					EntityType::SkeletonHorseman,
					nullptr,
					FTransform { rotation, location, FVector::OneVector },
					true,
					false
				);
			}
		};
	}
}}