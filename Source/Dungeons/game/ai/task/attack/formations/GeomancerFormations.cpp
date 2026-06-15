#include "Dungeons.h"
#include "GeomancerFormations.h"
#include "game/Conversion.h"
#include "game/actor/character/mob/MobParams.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include "util/Random.h"

namespace bt { namespace formation {

	FTransform rotateRandomly(const FVector& translation) {
		static Random rnd;

		return {
			FRotator { 0.f, rnd.nextInt(4) * 90.f, 0.f },
			translation,
			FVector::OneVector
		};
	}	

	struct GridEntry {
		int x;
		int y;
		float time;
	};
	
	void addEntry(
		bt::Duration delay,
		EntityType type,
		FVector location,
		float time,
		bool transferEnchantments,
		USummonFormationTask::AddEntryRef add
	) {
		static TArray<GridEntry> entries;

		if (entries.Num() > 20) {
			entries.RemoveAllSwap([&](const auto& candidate) { return time - candidate.time > 5.f; });
		}

		const auto pos = conversion::ueToBlock(location);

		if (!entries.FindByPredicate([&](const auto& candidate) { return candidate.x == pos.x && candidate.y == pos.z; })) {
			entries.Add({ pos.x, pos.z, time });
			add(delay, type, nullptr, rotateRandomly(location), false, transferEnchantments);
		}
	}

	void GeomancerWallLine(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		// move these things to a better place
		const FVector OneX { 1, 0, 0 };
		const FVector OneY { 0, 1, 0 };
		const FVector OneZ { 0, 0, 1 };

		const auto halfLength { 4 };

		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto delta = state.owner->GetActorLocation() - target->GetActorLocation();
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));;

			if (FMath::Abs(delta.X) > FMath::Abs(delta.Y)) {
				const auto origin = targetLocation - OneX * 1 * Math::PE_TO_UE_UNITS * FMath::Sign(delta.X);
				
				for (auto i = -halfLength; i <= halfLength; i++) {
					const auto location = origin + OneY * i * Math::PE_TO_UE_UNITS;
					
					addEntry(
						FMath::Abs(i) * .06s,
						EntityType::GeomancerWall,
						location,
						state.world().GetTimeSeconds(),
						false,
						add
					);
				}
			} else {
				const auto origin = targetLocation - OneY * 1.0f * Math::PE_TO_UE_UNITS * FMath::Sign(delta.Y);
				
				for (auto i = -halfLength; i <= halfLength; i++) {
					const auto location = origin + OneX * i * Math::PE_TO_UE_UNITS;
					
					addEntry(
						FMath::Abs(i) * .06s,
						EntityType::GeomancerWall,
						location,
						state.world().GetTimeSeconds(),
						false,
						add
					);
				}
			}
		}
	}

	void GeomancerWallAround(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		// switch to a more maintainable format
		static const TArray<TArray<FVector2D>> fenceFormations {
			{ { -2, -1 }, { -2, 0 }, { -2, 1 },   { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 },   { 2, -1 }, { 2, 0 }, { 2, 1 } },
			{ { -1, -2 }, { 0, -2 }, { 1, -2 },   { -2, -2 }, { -2, -1 }, { -2, 0 }, { -2, 1 }, { -2, 2 },   { -1, 2 }, { 0, 2 }, { 1, 2 } },
			{ { -2, -1 }, { -2, 0 }, { -2, 1 },   { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 },   { 2, -1 }, { 2, 0 }, { 2, 1 } },
			{ { -1, 2 }, { 0, 2 }, { 1, 2 },   { 2, -2 }, { 2, -1 }, { 2, 0 }, { 2, 1 }, { 2, 2 },   { -1, -2 }, { 0, -2 }, { 1, -2 } }
		};

		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto forward = target->GetActorForwardVector();

			const auto formationIndex = [&] {
				if (FMath::Abs(forward.X) > FMath::Abs(forward.Y)) {
					return forward.X < 0 ? 1 : 3;
				} else {
					return forward.Y < 0 ? 0 : 2;
				}
			}();

			const auto& formation = fenceFormations[formationIndex];
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			for (auto i = 0; i < formation.Num(); i++) {
				const auto worldLocation = FVector { formation[i], 0 } * 1.0f * Math::PE_TO_UE_UNITS + targetLocation;
				
				addEntry(
					FMath::Abs(i - formation.Num() / 2.f) * .08s,
					EntityType::GeomancerWall,
					worldLocation,
					state.world().GetTimeSeconds(),
					false,
					add
				);
			}
		}
	}

	void GeomancerWallRandom(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		static Random rnd;
		const int count { 7 };
		const float radiusMax { 7 };
		const float radiusMin { 2 };

		// this sort of set should be shared with all geomancers so that they don't summon over each other
		// the problem is that the set would need cleaning up
		TSet<FVector2D> locations;

		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			while (locations.Num() < count) {
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
					const auto worldLocation = FVector { candidate, 0 } * Math::PE_TO_UE_UNITS + targetLocation;
					
					addEntry(
						(locations.Num() % 3) * .12s,
						EntityType::GeomancerWall,
						worldLocation,
						state.world().GetTimeSeconds(),
						false,
						add
					);
				}
			}
		}
	}

	void GeomancerBombSolo(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			const auto ownerLocation = state.owner->GetActorLocation();
			static Random rnd;

			const auto location = [&] {
				const auto delta = targetLocation - ownerLocation;		
				const auto offsetX = Math::PE_TO_UE_UNITS * 2.f * FMath::Sign(rnd.nextGaussianFloat());
				const auto offsetY = Math::PE_TO_UE_UNITS * 2.f * FMath::Sign(rnd.nextGaussianFloat());
				return FVector { offsetX, offsetY, 0.f };				
			}();

			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + location, state.world().GetTimeSeconds(), false, add);
		}
	}

	void GeomancerBombMultiple(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			const auto offset = Math::PE_TO_UE_UNITS * 3.0f;

			addEntry(0.s, EntityType::GeomancerBomb, targetLocation - FVector { 0.f, offset, 0.f }, state.world().GetTimeSeconds(), false, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation - FVector { offset, 0.f, 0.f }, state.world().GetTimeSeconds(), false, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + FVector { 0.f, offset, 0.f }, state.world().GetTimeSeconds(), false, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + FVector { offset, 0.f, 0.f }, state.world().GetTimeSeconds(), false, add);
		}
	}

	void FrostWardenBombSolo(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			const auto ownerLocation = state.owner->GetActorLocation();
			static Random rnd;

			const auto location = [&] {
				const auto delta = targetLocation - ownerLocation;
				const auto offsetX = Math::PE_TO_UE_UNITS * 2.f * FMath::Sign(rnd.nextGaussianFloat());
				const auto offsetY = Math::PE_TO_UE_UNITS * 2.f * FMath::Sign(rnd.nextGaussianFloat());
				return FVector{ offsetX, offsetY, 0.f };
			}();

			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + location, state.world().GetTimeSeconds(), true, add);
		}
	}

	void FrostWardenBombMultiple(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		if (state.params().target.HasActor()) {
			const AActor* target = state.params().target;
			const auto targetLocation = conversion::blockCenterXZToUe(conversion::ueToBlock(target->GetActorLocation()));
			const auto offset = Math::PE_TO_UE_UNITS * 3.0f;

			addEntry(0.s, EntityType::GeomancerBomb, targetLocation - FVector{ 0.f, offset, 0.f }, state.world().GetTimeSeconds(), true, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation - FVector{ offset, 0.f, 0.f }, state.world().GetTimeSeconds(), true, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + FVector{ 0.f, offset, 0.f }, state.world().GetTimeSeconds(), true, add);
			addEntry(0.s, EntityType::GeomancerBomb, targetLocation + FVector{ offset, 0.f, 0.f }, state.world().GetTimeSeconds(), true, add);
		}
	}

	void GeomancerAll(bt::StateRef state, USummonFormationTask::AddEntryRef add) {
		static Random rnd;

		// compose add with another function to fix the rotations
		switch (rnd.nextInt(2)) {
		case 0: return GeomancerWallRandom(state, add);
		case 1: return GeomancerWallAround(state, add);
		//case 2: return GeomancerBombSolo(state, add);
		//case 3: return GeomancerWallLine(state, add);
		default: checkNoEntry();
		}
	}

}}
