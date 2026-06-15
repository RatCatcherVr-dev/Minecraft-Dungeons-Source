#include "Dungeons.h"
#include "RedstoneFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Algo.h"
#include "util/Random.h"


namespace bt { namespace formation {
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeRedstoneCubeRandom(float radiusMin, float radiusMax, int count, EntityType mobType) {
		static Random rnd;
		
		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			TSet<FVector2D> locations;

			const AActor* target = state.owner;

			while (locations.Num() < count) {
				// it's hardly noticeable that the cubes spawn on the grid; might just go for free locations
				const FVector2D candidate {
					static_cast<float>(rnd.nextInt(-radiusMax, radiusMax + 1)),
					static_cast<float>(rnd.nextInt(-radiusMax, radiusMax + 1))
				};

				if (
					candidate.Size() < radiusMax &&
					candidate.Size() >= radiusMin &&
					!locations.Contains(candidate)
				) {
					locations.Add(candidate);
					const auto worldLocation = FVector { candidate, 0 } * 100 + target->GetActorLocation();
					
					add(
						(locations.Num() % 3) * .25s,
						mobType,
						nullptr,
						FTransform { worldLocation },
						false,
						false
					);
				}
			}
		};
	}

	// these two are the same now but keep them separate as they will diverge
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeRedstoneDotRandom(
		const TSubclassOf<AActor>& dotClass,
		float radiusMin,
		float radiusMax,
		float dotRadius,
		float dotDelay,
		float sectionRadius,
		float sectionDelay,
		int count
	) {
		static Random rnd;

		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			TArray<FVector2D> locations;

			const AActor* target = state.owner;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			int tries = 0;
			while (locations.Num() < count) {
				// it's hardly noticeable that the cubes spawn on the grid; might just go for free locations
				const FVector2D candidate{
					static_cast<float>(rnd.nextInt(-radiusMax, radiusMax + 1)),
					static_cast<float>(rnd.nextInt(-radiusMax, radiusMax + 1))
				};


				if (
					candidate.Size() < radiusMax &&
					candidate.Size() >= radiusMin &&
					//!locations.Contains(candidate)
					algo::none_of(locations, RETLAMBDA( (FMath::Abs(it.X-candidate.X) <= dotRadius) && (FMath::Abs(it.Y - candidate.Y) <= dotRadius)))
				) {
					tries = 0;
					locations.Add(candidate);
					const auto worldLocation = FVector{ candidate, 0 } * 100 + targetLocation;
					
					add(
						//	(locations.Num() % 3) * .25s,
						Seconds(locations.Num()*dotDelay + FMath::FloorToFloat(candidate.Size() / sectionRadius)*sectionDelay),
						EntityType::Undefined,
						dotClass,
						FTransform { worldLocation },
						false,
						false
					);
				} else {
					if (tries++ > 50)
					{
						break;
					}
				}

			}
		};
	}
}}
