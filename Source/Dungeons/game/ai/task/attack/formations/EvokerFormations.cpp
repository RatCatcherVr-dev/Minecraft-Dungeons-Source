#include "Dungeons.h"
#include "game/actor/character//mob/MobCharacter.h"
#include "EvokerFormations.h"
#include "util/Random.h"

namespace bt { namespace formation {
	
	void EvokerFangLine(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		const int count { 7 };		
		const float spacing { 160.f };
		const float offset { 200.f };

		if (state.params().target.HasActor()) {
			const auto ownerLocation = state.owner->GetActorLocation();

			const AActor* target = state.params().target;
			const auto targetLocation = target->GetActorLocation();
			
			const auto direction = [&] {
				auto delta = targetLocation - ownerLocation;
				delta.Z = 0;
				delta.Normalize();

				return delta;
			}();			

			for (auto i = 0; i < count; i++) {
				const auto location = ownerLocation + direction * (spacing * i + offset);

				const float yaw = FMath::Atan2(ownerLocation.Y - targetLocation.Y, ownerLocation.X - targetLocation.X);

				const FTransform transform {
					FRotator { 0.f, FMath::RadiansToDegrees(yaw), 0.f },
					location,
					FVector::OneVector
				};

				add(0.06s * i, EntityType::EvokerFang, nullptr, transform, true, false);
			}
		}
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeEvokerFangCircle(const Provider<int>& countProvider, const Provider<float>& radiusProvider) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const int count = countProvider(state);
			const int halfCount { count / 2 };
			const float angle = 360.f / count;
			const float radius = radiusProvider(state);

			const auto rotation = state.owner->GetActorRotation();

			for (auto i = 0; i < count; i++) {
				const auto location = state.owner->GetActorLocation() + state.owner->GetActorForwardVector().RotateAngleAxis(angle * i, FVector::UpVector) * radius;

				const FTransform transform {
					{ 0.f, rotation.Yaw + angle * i, 0.f },
					location,
					FVector::OneVector
				};

				add(0.06s * (halfCount - FMath::Abs(i - halfCount)), EntityType::EvokerFang, nullptr, transform, true, false);
			}
		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeEvokerVexAround(int count, float radius) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const int halfCount { count / 2 };
			const float angle = 360.f / count;			

			const auto rotation = state.owner->GetActorRotation();

			for (auto i = 0; i < count; i++) {
				const auto location = state.owner->GetActorLocation() + state.owner->GetActorForwardVector().RotateAngleAxis(angle * i, FVector::UpVector) * radius;

				const FTransform transform {
					{ 0.f, rotation.Yaw + angle * i, 0.f },
					location,
					FVector::OneVector
				};

				add(0.06s * (halfCount - FMath::Abs(i - halfCount)), EntityType::Vex, nullptr, transform, true, false);
			}
		};
	}

}}
