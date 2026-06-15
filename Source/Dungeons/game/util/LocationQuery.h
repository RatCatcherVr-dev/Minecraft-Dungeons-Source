#pragma once

#include "game/actor/character/BaseCharacter.h"
#include "lovika/tile/TileDef.h"

class DoorDef;

namespace locationquery {
	bool isOverlappingPlayers(const UWorld& world, const FVector& location, float radius = 50.f);

	bool isInFrontOf(const AActor* actor, const FVector& location, float angle = HALF_PI);
	
	TOptional<FVector> getRandomLocationAround(const UWorld& world, const FVector& center, float radius = 300.f);

	TOptional<FVector> getRandomLocationAround(const UWorld& world, const ABaseCharacter& origin, const FVector& center, float radius = 300.f);

	template<typename Generator>
	auto retry(const Generator& generator, int triesMax = 10) -> decltype(generator()) {
		for (int triesCount = 0; triesCount < triesMax; triesCount++) {
			if (auto candidate = generator()) {
				return candidate;
			}
		}
		return {};
	}

	template<typename Generator, typename Predicate>
	auto retry(const Generator& generator, const Predicate& predicate, int triesMax = 10) -> decltype(generator()) {
		for (int triesCount = 0; triesCount < triesMax; triesCount++) {
			const auto candidate = generator();

			if (candidate.IsSet() && predicate(candidate.GetValue())) {
				return candidate;
			}
		}

		return {};
	}

	bool isOnGround(BlockSource&, const FVector worldLocation);
	bool isOnGround(BlockSource&, const BlockPos&);
	bool isOnGround(BlockSource&, const ACharacter*);

	TOptional<float> findGround(BlockSource& blockSource, FVector worldLocation);
	TOptional<float> findGround(const UWorld& world, FVector worldLocation, bool ignoreCharacters = false);
	TOptional<float> findGround(const UWorld& world, BlockSource& blockSource, FVector worldLocation);

	template <typename ...Terrain>
	TOptional<float> findGround(Terrain&... terrain, ACharacter* character) {
		const auto capsule = character->GetCapsuleComponent();
		const auto capsuleBottom = capsule->GetComponentLocation() - FVector { 0.f, 0.f, capsule->GetScaledCapsuleHalfHeight() };

		if (const auto ground = findGround(terrain..., capsuleBottom)) {
			return { ground.GetValue() + capsule->GetScaledCapsuleHalfHeight() };
		}
		return {};
	}

	bool isLocationOnSolidBlock(const UWorld&, const FVector& worldLocation);
	TArray<FVector> findGroundBlockPositions(UWorld&, BlockCuboid, TOptional<BlockPos> minSize = {});
	TArray<FVector> findGroundBlockPositions(UWorld&, const FVector& center, const FVector& minSize);

	TArray<FVector> findNavMeshPositions(UWorld&, BlockCuboid, TOptional<BlockPos> minSize = {});
	TArray<FVector> findNavMeshPositions(UWorld&, const FVector& center, const FVector& minSize);
}
