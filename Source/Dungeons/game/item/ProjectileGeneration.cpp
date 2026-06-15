#include "Dungeons.h"
#include "game/item/ProjectileGeneration.h"
#include <GameFramework/ProjectileMovementComponent.h>

FProjectileElement::FProjectileElement(FTransform spawnTransform, FVector velocity, TSubclassOf<AActor> actorTypeToSpawn)
	: SpawnTransform(std::move(spawnTransform))
	, Velocity(std::move(velocity))
	, ActorTypeToSpawn(actorTypeToSpawn) {
}

namespace projectilegeneration {

TArray<AActor*> Spawn(AActor* owner, const TArray<FProjectileElement>& elements) {
	TArray<AActor*> spawned;
	for (auto& elem : elements) {
		const auto spawnTransform = elem.SpawnTransform;
		auto actor = owner->GetWorld()->SpawnActorDeferred<AActor>(elem.ActorTypeToSpawn, spawnTransform, owner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		UGameplayStatics::FinishSpawningActor(actor, spawnTransform);
		auto projectileMovement = actor ? actor->FindComponentByClass<UProjectileMovementComponent>() : nullptr;
		if (projectileMovement) {
			projectileMovement->Velocity = elem.Velocity;
		}
		spawned.Add(actor);
	}
	return spawned;
}

FProjectileElement generate(const ProjectileGenerator& projectileGen, Random* random) {
	return FProjectileElement(projectileGen.TransformGen(random), projectileGen.VelocityGen(random), projectileGen.TypeGen(random));
}

TArray<FProjectileElement> GenerateElementsFrom(AActor* owner, int amount, const ProjectileGenerator& generator, Random* random) {
	TArray<FProjectileElement> elements;
	for (int i = 0; i < amount; i++) {
		elements.Add(generate(generator, random));
	}
	return elements;
}

TransformGenerator TransformFromActor(AActor* actor, FVector offset) {
	return [actor, offset](Random* random)  {
		auto randRot = [&]() { return FloatRange(0.f, 360.f).random(random); };
		auto rotation = FRotator(randRot(), randRot(), randRot());
		return FTransform(rotation, actor->GetActorLocation() + offset, FVector(1));
	};
}

VectorGenerator RandomVelocityAround(const FloatRange& velocityRange) {
	return [velocityRange] (Random* random)  {
		auto dir = FVector(FloatRange(-1.f, 1.f).random(random), FloatRange(-1.f, 1.f).random(random), FloatRange(0.f, 1.f).random(random)); dir.Normalize();
		return dir * velocityRange.random(random);
	};
}

VectorGenerator RandomVelocityInCone(const FloatRange& velocityRange, AActor* origin, const FVector& directionFromOrigin, float angle) {
	return [velocityRange, origin, directionFromOrigin, angle] (Random* random) {
		const auto forward = origin->GetActorForwardVector();
		auto fromOrigin = directionFromOrigin; fromOrigin.Normalize();
		auto dir = forward.Rotation().RotateVector(fromOrigin);
		dir.Z = (dir.Z < 0.f && fromOrigin.Z > 0.f) || (dir.Z > 0.f && fromOrigin.Z < 0.f) ? dir.Z * -1 : dir.Z;
		FloatRange angleSpan(-angle, angle);
		auto rotatedDir = FRotator(angleSpan.random(random), angleSpan.random(random), angleSpan.random(random)).RotateVector(dir);
		return rotatedDir * velocityRange.random(random);
	};
}

}
