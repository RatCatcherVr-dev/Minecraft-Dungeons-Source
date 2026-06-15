#include "Dungeons.h"
#include "CauldronFormations.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "CommonTypes.h"

namespace bt { namespace formation {

	struct PlayerInfo {
		int count;
		FVector center;
	};

	PlayerInfo getPlayersInfo(UWorld& world, const FVector& origin) {
		FVector average { 0.f, 0.f, 0.f };
		int count { 0 };
		for (auto&& player : TActorRange<APlayerCharacter>(&world)) {
			const auto playerLocation = player->GetActorLocation();
			if (FVector::DistSquared2D(playerLocation, origin) < FMath::Square(2000.f)) {
				average += playerLocation;
				count++;
			}
		}

		return {
			count,
			count > 0 ? average / count : origin
		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchLine(int halfCountMin, float spacing, float offsetFront) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const auto playersInfo = getPlayersInfo(state.world(), state.owner->GetActorLocation());

			const auto rotation = state.owner->GetActorRotation();
			const auto lineLocation = (state.owner->GetActorLocation() + playersInfo.center) * .5f;
			const auto lateral = state.owner->GetActorForwardVector().RotateAngleAxis(90, FVector::UpVector);

			add(0.s, EntityType::PillagerVariant0, nullptr, FTransform { rotation, lineLocation, FVector::OneVector }, false, false);

			const int halfCount = halfCountMin + playersInfo.count;
			for (auto i = 1; i <= halfCount; i++) {
				{
					const auto location = lineLocation + lateral * (spacing * i);
					add(0.06s * i, EntityType::PillagerVariant0, nullptr, FTransform { rotation, location, FVector::OneVector }, false, false);
				}

				{
					const auto location = lineLocation - lateral * (spacing * i);
					add(0.06s * i, EntityType::PillagerVariant0, nullptr, FTransform { rotation, location, FVector::OneVector }, false, false);
				}
			}
		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchCircle(int countMin) {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const auto ownerLocation = state.owner->GetActorLocation();
			const auto playersInfo = getPlayersInfo(state.world(), ownerLocation);
			
			const int count = countMin + playersInfo.count * 2;
			const int halfCount { count / 2 };
			const float angle = 360.f / count;
			const float radius = 700.f + playersInfo.count * 70.f;

			const auto rotation = state.owner->GetActorRotation();

			for (auto i = 0; i < count; i++) {
				const auto location = playersInfo.center + state.owner->GetActorForwardVector().RotateAngleAxis(angle * i, FVector::UpVector) * radius;

				const FTransform transform {
					{ 0.f, rotation.Yaw + angle * i, 0.f },
					location,
					FVector::OneVector
				};

				add(0.06s * (halfCount - FMath::Abs(i - halfCount)), EntityType::VindicatorVariant0, nullptr, transform, true, false);
			}
		};
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchSingle() {
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const auto ownerLocation = state.owner->GetActorLocation();
			const auto playersInfo = getPlayersInfo(state.world(), ownerLocation);
			
			const auto yaw = FRotationMatrix::MakeFromX(playersInfo.center - ownerLocation).Rotator().Yaw;
			
			const FTransform transform {
				{ 0.f, yaw, 0.f },
				(playersInfo.center + ownerLocation) * .5f,
				FVector::OneVector
			};

			add(.1s, EntityType::RedstoneGolem, nullptr, transform, true, false);			
		};
	}

}}