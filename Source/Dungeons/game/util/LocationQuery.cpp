#include "Dungeons.h"
#include "LocationQuery.h"
#include "ActorQuery.h"
#include <WorldCollision.h>
#include <Engine.h>
#include "game/component/TeleportComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include <NavigationSystem.h>
#include <Optional.h>

namespace locationquery {
	bool isOverlappingPlayers(const UWorld& world, const FVector& location, float radius/* = 50.f*/) {
		return world.OverlapAnyTestByChannel(
			location,
			FQuat::Identity,
			(ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly,
			FCollisionShape::MakeSphere(radius)
		);
	}

	bool isInFrontOf(const AActor* actor, const FVector& location, float angle/* = HALF_PI */) {
		const auto actorForward = actor->GetActorForwardVector();
		const auto actorToTarget = location - actor->GetActorLocation();

		return (actorForward | actorToTarget.GetSafeNormal2D()) > FMath::Cos(angle);
	}

	TOptional<FVector> getRandomLocationAround(const UWorld& world, const FVector& center, float radius/* = 300.f*/) {
		const auto randomLocationAround = [&]() -> TOptional<FVector> {
			FNavLocation result;
			const bool found = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&world)->GetRandomReachablePointInRadius(center, radius, result);

			if (found) {
				return { result.Location };
			}

			return {};
		};

		return retry(randomLocationAround);
	}

	TOptional<FVector> getRandomLocationAround(const UWorld& world, const ABaseCharacter& origin, const FVector& center, float radius/* = 300.f*/) {
		const auto randomLocationAround = [&]() -> TOptional<FVector> {
			FNavLocation result;
			const auto nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&world);
			const bool found = nav->GetRandomReachablePointInRadius(center, radius, result);

			if (found) {
				return { result.Location };
			}

			return {};
		};

		const auto isReachableFromOrigin = [&](const FVector& location) {
			return origin.IsLocationReachable(location);
		};

		return retry(randomLocationAround, isReachableFromOrigin);
	}

	bool isOnGround(BlockSource& blockSource, const BlockPos& pos) {
		if (!blockSource.getBlock(pos.below()).isSolid()) {
			return false;
		}
		const auto current = blockSource.getBlock(pos);
		return !current.isSolid() && !current.isLiquid();
	}

	bool isOnGround(BlockSource& blockSource, const FVector worldLocation) {
		//Traces and character ground location returns a location that is on the edge so we have to nudge z a bit down.
		return isOnGround(blockSource, conversion::ueToBlock(worldLocation + FVector{ 0.0f, 0.0f, 1.0f }));
	}

	bool isOnGround(BlockSource& blockSource, const ACharacter* character) {
		const auto capsule = character->GetCapsuleComponent();
		const auto capsuleBottom = capsule->GetComponentLocation() - FVector { 0.f, 0.f, capsule->GetScaledCapsuleHalfHeight() };

		return isOnGround(blockSource, capsuleBottom);
	}

	TOptional<float> findGround(BlockSource& blockSource, FVector worldLocation) {
		// NB!: the terrain can have holes (the spaces under bridges)

		if (isOnGround(blockSource, worldLocation)) {
			return { worldLocation.Z };
		}

		const auto originalPos = conversion::ueToBlock(worldLocation);

		if (blockSource.getBlock(originalPos).isSolid()) {
			const auto triesMax { 8 };

			for (auto tries = 0; tries < triesMax; tries++) {
				const auto above = blockSource.getBlock(originalPos.above(tries));

				if (above.isLiquid()) {
					return {};
				}

				if (!above.isSolid()) {
					return { conversion::blockToUe(originalPos.above(tries)).Z };
				}
			}
		} else {
			const auto triesMax { 4 };

			for (auto tries = 0; tries < triesMax; tries++) {
				const auto below = blockSource.getBlock(originalPos.below(tries));

				if (below.isLiquid()) {
					return {};
				}

				if (below.isSolid()) {
					return { conversion::blockToUe(originalPos.below(tries)).Z };
				}
			}
		}

		return {};
	}

	TOptional<float> findGround(const UWorld& world, FVector worldLocation, bool ignoreCharacters/* = false*/) {
		const std::array<float, 4> offsets { 70.f, 160.f, 350.f, 940.f };
		const auto end = worldLocation - FVector { 0.f, 0.f, 900.f };

		// mobs block on TerrainAndPlayerOnly despite the name
		const auto traceChannel{ static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainAndPlayerOnly) };

		if (ignoreCharacters) {
			// there must be a better way to ignore characters
			auto& CharList = InstanceTracker< ABaseCharacter >::GetList((UWorld*)&world);

			FCollisionQueryParams queryParams;
			for (auto pChar : CharList)
			{
				queryParams.AddIgnoredActor(pChar);
			}

			FHitResult hitResult;

			for (auto offset : offsets) {
				const auto start = worldLocation + FVector { 0.f, 0.f, offset };
				if (world.LineTraceSingleByChannel(hitResult, start, end, traceChannel, queryParams)) {
					return { hitResult.Location.Z };
				}
			}
		}
		else {
			FHitResult hitResult;

			for (auto offset : offsets) {
				const auto start = worldLocation + FVector{ 0.f, 0.f, offset };

				if (world.LineTraceSingleByChannel(hitResult, start, end, traceChannel)) {
					if (hitResult.Actor != nullptr && !hitResult.Actor->IsA<ABaseCharacter>()) {
						return { hitResult.Location.Z };
					}
				}
			}
		}

		return {};
	}

	TOptional<float> findGround(const UWorld& world, BlockSource& blockSource, FVector worldLocation) {
		const auto radius { 40.f };
		static const TArray<FVector> offsets {
			{ -radius, 0.f, 0.f },
			{ 0.f, -radius, 0.f },
			{ radius, 0.f, 0.f },
			{ 0.f, radius, 0.f }
		};

		auto max { -std::numeric_limits<float>::infinity() };

		for (auto&& offset : offsets) {
			if (const auto traceResult = findGround(world, worldLocation + offset)) {
				const auto candidate = traceResult.GetValue();
				if (isOnGround(blockSource, FVector{ worldLocation.X, worldLocation.Y, candidate })) {
					max = FMath::Max(max, candidate);
				}
			} else {
				return {};
			}
		}

		return { max };
	}

	bool isLocationOnSolidBlock(const UWorld& world, const FVector& worldLocation) {
		auto* game = actorquery::getFirstActor<AGameBP>(GEngine->GetWorldFromContextObjectChecked(&world));
		return isOnGround(*game->BlockSource(), worldLocation);
	}

	void extendToMinSize(BlockCuboid& area, BlockPos minSize) {
		const auto size = area.size();
		const auto newSize = BlockPos::max(size, minSize);
		const auto delta = BlockPos::max(newSize - size, BlockPos::ZERO);
		const auto halfDelta = BlockPos{ delta.x / 2, delta.y / 2, delta.z / 2 };
		area.minInclusive -= halfDelta;
		area.maxExclusive += (delta - halfDelta); // @note: We want the increase to sum up to <delta>
	}

	TArray<FVector> findGroundBlockPositions(UWorld& world, BlockCuboid area, TOptional<BlockPos> minSize) {
		if (minSize) {
			extendToMinSize(area, minSize.GetValue());
		}
		const auto* game = actorquery::getFirstActor<AGameBP>(&world);
		auto* region = game ? game->BlockSource() : nullptr;

		return region ?
			algo::copy_if_map_tarray(area, RETLAMBDA(isOnGround(*region, it) && !region->getBlock(it.above()).isSolid()), conversion::blockCenterXYZToUe) :
			algo::map_tarray(area, conversion::blockCenterXYZToUe);
	}

	TArray<FVector> findNavMeshPositions(UWorld& world, BlockCuboid area, TOptional<BlockPos> minSize) {
		if (minSize) {
			extendToMinSize(area, minSize.GetValue());
		}

		const auto nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&world);
		FNavLocation out;

		TArray<FVector> positions;
		positions.Reserve(area.volume());
		for (auto block : area) {
			if (nav->ProjectPointToNavigation(conversion::blockToUe(block), out, FVector{ 25,25,125 })) {
				positions.Add(out);
			}
		}

		return positions;
	}

	TArray<FVector> findGroundBlockPositions(UWorld& world, const FVector& center, const FVector& minSize) {
		return findGroundBlockPositions(world, BlockCuboid::fromPositionAndSize(conversion::ueToBlock(center - minSize * 0.5f), conversion::ueToBlock(minSize)));
	}

	TArray<FVector> findNavMeshPositions(UWorld& world, const FVector& center, const FVector& minSize) {
		return findNavMeshPositions(world, BlockCuboid::fromPositionAndSize(conversion::ueToBlock(center - minSize * 0.5f), conversion::ueToBlock(minSize)));
	}
}