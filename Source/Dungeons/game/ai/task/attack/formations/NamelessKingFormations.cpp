#include "Dungeons.h"
#include "game/actor/character//mob/MobCharacter.h"
#include "NamelessKingFormations.h"

namespace bt { namespace formation {

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> NamelssKingDefensive(int halfCount, float spacing) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const float offset { 300.f };

			const auto rotation = state.owner->GetActorRotation();
			const auto frontLocation = state.owner->GetActorLocation() + state.owner->GetActorForwardVector() * offset;
			const auto lateral = state.owner->GetActorForwardVector().RotateAngleAxis(90, FVector::UpVector);

			add(0.s, EntityType::SkeletonVanguard, nullptr, FTransform { rotation, frontLocation, FVector::OneVector }, false, false);

			for (auto i = 1; i <= halfCount; i++) {
				{
					const auto location = frontLocation + lateral * (spacing * i);
					add(0.06s * i, EntityType::SkeletonVanguard, nullptr, FTransform { rotation, location, FVector::OneVector }, false, false);
				}

				{
					const auto location = frontLocation - lateral * (spacing * i);
					add(0.06s * i, EntityType::SkeletonVanguard, nullptr, FTransform { rotation, location, FVector::OneVector }, false, false);
				}
			}
		};
	}

}}