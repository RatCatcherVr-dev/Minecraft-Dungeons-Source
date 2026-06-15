#include "Dungeons.h"
#include "VesselFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"
#include "game/actor/character/player/PlayerCharacter.h"

namespace bt { namespace formation {

	TArray<AActor*> getNearbyPlayers(UWorld* world, FVector location) {
		TArray<AActor*> nearbyPlayers;

		for (auto* player : TActorRange<APlayerCharacter>(world)) {
			if (FVector::DistSquared2D(player->GetActorLocation(), location) < FMath::Square(2500.f)) {
				nearbyPlayers.Add(player);
			}
		}

		return nearbyPlayers;
	}

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeVesselRandom(
		int countExtras,
		bt::Duration totalTime
	) {
		static Random rnd;		

		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			const auto addVisage = [&](FVector location, float yaw, bt::Duration delay) {
				add(
					delay,
					EntityType::ArchVisage,
					nullptr,
					FTransform { FRotator { 0.f, yaw, 0.f }, location, FVector::OneVector },
					true,
					false
				);
			};

			const auto rndSign = [&] { return rnd.nextBoolean() ? 1.f : -1.f; };

			const auto axisOffsetMin = 4;
			const auto axisOffsetMax = 12;

			std::array<std::function<void(FVector, int, bt::Duration)>, 4> adders {
				[&](FVector targetLocation, int offsetMultiplier, bt::Duration delay) {
					addVisage(
						targetLocation - FVector { offsetMultiplier * 100.f * rndSign(), rnd.nextInt(axisOffsetMin, axisOffsetMax) * 100.f, 0.f },
						90.f,
						delay
					);
				},
				[&](FVector targetLocation, int offsetMultiplier, bt::Duration delay) {
					addVisage(
						targetLocation - FVector { rnd.nextInt(axisOffsetMin, axisOffsetMax) * 100.f, offsetMultiplier * 100.f * rndSign(), 0.f },
						0.f,
						delay
					);
				},
				[&](FVector targetLocation, int offsetMultiplier, bt::Duration delay) {
					addVisage(
						targetLocation + FVector { offsetMultiplier * 100.f * rndSign(), rnd.nextInt(axisOffsetMin, axisOffsetMax) * 100.f, 0.f },
						270.f,
						delay
					);
				},
				[&](FVector targetLocation, int offsetMultiplier, bt::Duration delay) {
					addVisage(
						targetLocation + FVector { rnd.nextInt(axisOffsetMin, axisOffsetMax) * 100.f, offsetMultiplier * 100.f * rndSign(), 0.f },
						180.f,
						delay
					);
				}
			};
			
			{
				const auto nearbyPlayers = getNearbyPlayers(&state.world(), state.owner->GetActorLocation());
				if (nearbyPlayers.Num() <= 0) {
					return;
				}

				const auto delayBetween = totalTime / (nearbyPlayers.Num() + countExtras);
				
				int adderIndexOffset { rnd.nextInt(adders.size()) };
				int spawnIndex { 0 };

				for (auto* player : nearbyPlayers) {
					const int adderIndex = (spawnIndex + adderIndexOffset) % adders.size();
					adders[adderIndex](player->GetActorLocation(), 0, delayBetween * spawnIndex);
					
					spawnIndex++;
				}

				for (int i = 0; i < countExtras; i++) {
					const auto player = nearbyPlayers[rnd.nextInt(0, nearbyPlayers.Num())];					
					const int adderIndex = (spawnIndex + adderIndexOffset) % adders.size();

					adders[adderIndex](player->GetActorLocation(), 3, delayBetween * spawnIndex);					

					spawnIndex++;
				}
			}
		};
	}

}}
