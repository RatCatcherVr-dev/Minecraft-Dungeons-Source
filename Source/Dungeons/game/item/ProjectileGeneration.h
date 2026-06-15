#pragma once
#include "util/FloatRange.h"
#include "util/Random.h"
#include "ProjectileGeneration.generated.h"

USTRUCT()
struct FProjectileElement {
	GENERATED_BODY()
	FProjectileElement() {}
	FProjectileElement(FTransform SpawnTransform, FVector Velocity, TSubclassOf<AActor> ActorTypeToSpawn);
	UPROPERTY()
	FTransform SpawnTransform;
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	TSubclassOf<AActor> ActorTypeToSpawn;
};

namespace projectilegeneration {
	using TransformGenerator = std::function<FTransform(Random*)>;
	using VectorGenerator = std::function<FVector(Random*)>;
	using TypeGenerator = std::function<TSubclassOf<AActor>(Random*)>;

	struct ProjectileGenerator {
		TransformGenerator TransformGen;
		VectorGenerator VelocityGen;
		TypeGenerator TypeGen;
	};

	TArray<AActor*> Spawn(AActor* owner, const TArray<FProjectileElement>&);
	FProjectileElement generate(const ProjectileGenerator&, Random*);
	TArray<FProjectileElement> GenerateElementsFrom(AActor* owner, int amount, const ProjectileGenerator& generator, Random* random = nullptr);

	TransformGenerator TransformFromActor(AActor*, FVector offset);
	VectorGenerator RandomVelocityAround(const FloatRange& velocityRange);
	VectorGenerator RandomVelocityInCone(const FloatRange& velocityRange, AActor* origin, const FVector& directionFromOrigin, float angle);
}