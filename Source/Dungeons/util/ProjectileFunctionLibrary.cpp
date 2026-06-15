#include "ProjectileFunctionLibrary.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/ActorQuery.h"
#include <DrawDebugHelpers.h>
#include "game/actor/ProjectileManager.h"
#include "game/component/RagdollOnDeathComponent.h"

FRandomStream UProjectileFunctionLibrary::DefaultRandom = FRandomStream();
TArray<FName> UProjectileFunctionLibrary::AttachableBones = {
	FName("J_Head"),
	FName("J_Neck"),
	FName("J_Spine2"),
	FName("J_Shoulders"),
	FName("J_L_Arm"),
	FName("J_R_Arm"),
	FName("OuterCube"),
	FName("InnerCube"),
};


TAutoConsoleVariable<int32> CVarDebugDrawProjectileAttach(
	TEXT("Dungeons.DebugDraw.ProjectileAttach"),
	0,
	TEXT("Enables debug drawing of projectile attachment attempts.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),

	ECVF_Cheat);

ABaseProjectileProp* UProjectileFunctionLibrary::SpawnAttachedArrowProp(AActor* fromActor, AActor* attachTarget, TSubclassOf<ABaseProjectileProp> ArrowPropClass, FVector WorldCollisionPosition /*= FVector::ZeroVector*/) {
	///For when shooting a projectile from a location, get a good random homing offset.
	static auto RandomlyOffsetActorAttachmentLocation = [](FVector& attachLocation, const float maxFactor, const AActor* target, FRandomStream& randStream) {
		if (target) {
			if (const auto hitCharacter = Cast<ACharacter>(target)) {
				const FVector actorLocation = target->GetActorLocation();
				if (const auto hitCapsule = hitCharacter->GetCapsuleComponent()) {
					
					//base offset on the target radius and height.
					const float capsuleHalfHeight = hitCapsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
					const float capsuleRadius = hitCapsule->GetScaledCapsuleRadius();
					const float maxRandomOffset = FMath::Min(capsuleHalfHeight * maxFactor, capsuleRadius * maxFactor);

					//Randomize the offset along a gaussian distribution.
					const float gaussianRandom = randStream.FRand() - randStream.FRand();

					//Create a new absolute randomized Z
					const float absoluteRandomizedZ = attachLocation.Z + maxRandomOffset * gaussianRandom;

					//Convert to relative to target actor
					const float absoluteRandomizedZRelativeToActorZ = absoluteRandomizedZ - actorLocation.Z;

					//Clamp offset to the size of the capsule.
					const float clampedToTargetCapsuleZ = FMath::Clamp(absoluteRandomizedZRelativeToActorZ, -capsuleHalfHeight, capsuleHalfHeight);
					attachLocation.Z = actorLocation.Z + clampedToTargetCapsuleZ;
				}
			}
		}
	};

	USceneComponent* attachParent = nullptr;

	if (attachTarget->Tags.Contains("cannot-attach-projectiles")) {
		return nullptr;
	}

	const bool drawDebug = CVarDebugDrawProjectileAttach.GetValueOnGameThread()>0;
	FVector propSpawnLocation = (WorldCollisionPosition.IsZero()) ? fromActor->GetActorLocation() : WorldCollisionPosition;
	FName propAttachBoneName = "";

	if (auto Character = Cast<ABaseCharacter>(attachTarget)) {		
		
		if (auto* RagDollComp = Character->GetRagdollOnDeathComponent())
		{
			//if the character is dying/ragdolling dont attempt to attach to it as its physics state is in flux
			if(RagDollComp->IsRagdolling())
				return nullptr;
		}

		auto mesh = Character->GetMesh();

		//Use default shared random since this is a visual offset for prop attachment.
		RandomlyOffsetActorAttachmentLocation(propSpawnLocation, 1.0f, attachTarget, DefaultRandom);

		UPhysicsAsset* physicsAsset = mesh->GetPhysicsAsset();
		if (physicsAsset && !mesh->PhysicsAssetOverride) {
			//If we have default skeletal mesh physics asset - use that to find the closest bone to attach to
			if (!attachParent) {
				FName boneName;
				FVector boneLocation;
				if (actorquery::getClosestBoneOnPhysicsAsset(propSpawnLocation, attachTarget, boneName, boneLocation, false)) {
					if (drawDebug) {
						DrawDebugLine(attachTarget->GetWorld(), propSpawnLocation, boneLocation, FColor::Green, false, 8.0f);
					}
					attachParent = mesh;
					propAttachBoneName = boneName;
					propSpawnLocation = boneLocation;
				}
			}		
		}

		if (!attachParent) {
			//Fallback is to just 
			FName boneName;
			FVector boneLocation;
			if (actorquery::getClosestBoneFromList(propSpawnLocation, mesh, AttachableBones, boneName, boneLocation)) {
				if (drawDebug) {
					DrawDebugLine(attachTarget->GetWorld(), propSpawnLocation, boneLocation, FColor::Yellow, false, 8.0f);
				}
				attachParent = mesh;
				propAttachBoneName = boneName;
				propSpawnLocation = boneLocation;
			}
		}

		if (!attachParent) {
			//Failed to attach to anything
			return nullptr;
		}
	}

	auto fromActorTransform = fromActor->GetActorTransform();

	FTransform spawnTransform(fromActorTransform.GetRotation(), propSpawnLocation, fromActorTransform.GetScale3D());
		
	if (auto arrowProp = AProjectileActorManager::Pop_ProjectileProp(ArrowPropClass, spawnTransform, fromActor->GetWorld()) )
	{
		arrowProp->SetActorScale3D(fromActorTransform.GetScale3D());

		if (attachParent) {			
			arrowProp->AttachToComponent(attachParent, FAttachmentTransformRules::KeepWorldTransform, propAttachBoneName);
		}
		else if (attachTarget)
		{
			arrowProp->AttachToComponent(attachTarget->FindComponentByClass<UStaticMeshComponent>(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
		}

		return arrowProp;
	} else {
		if (drawDebug) {
			DrawDebugLine(attachTarget->GetWorld(), propSpawnLocation, propSpawnLocation + FVector(0.0f, 0.0f, 400.f), FColor::Red, false, 15.0f);
		}
	}

	return nullptr;
}

ABaseProjectileProp* UProjectileFunctionLibrary::SpawnProjectileProp(const UObject* WorldContextObject, TSubclassOf<ABaseProjectileProp> ArrowPropClass, const FTransform& SpawnTransform)
{
	const auto world = WorldContextObject->GetWorld();

	if (auto arrowProp = AProjectileActorManager::Pop_ProjectileProp(ArrowPropClass, SpawnTransform, world))
	{
		arrowProp->SetActorScale3D(SpawnTransform.GetScale3D());
		
				
		return arrowProp;
	}

	return nullptr;
}

void UProjectileFunctionLibrary::PreCacheProjectileProp(const UObject* WorldContextObject, TSubclassOf<ABaseProjectileProp> PropClass, int32 MinimumCachedCount /*= 16*/, int32 MaximumCachedCount /*= -1*/)
{
	const auto world = WorldContextObject->GetWorld();
	AProjectileActorManager::PreCache_ProjectilePropClass(PropClass, world, MinimumCachedCount, MaximumCachedCount);
}

void UProjectileFunctionLibrary::DeSpawnAttachedArrowProp(ABaseProjectileProp* ArrowPropActor)
{
	if (ArrowPropActor)
	{
		AProjectileActorManager::Push_ProjectileProp(ArrowPropActor);
	}
}

UParticleSystemComponent* UProjectileFunctionLibrary::SpawnHitParticleEffect(const UObject* WorldContextObject, class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation)
{
	const auto world = WorldContextObject->GetWorld();
	return AProjectileActorManager::SpawnHitParticleEffect(world, ParticleSystemTemplate, Location, Rotation);
}

void UProjectileFunctionLibrary::SpawnXPOrbs(const UObject* WorldContextObject, AActor* pSource, APlayerCharacter* pTarget, int32 iCount)
{
	const auto world = WorldContextObject->GetWorld();
	AProjectileActorManager::SpawnXPOrbs(world, pSource, pTarget, iCount);
}

void UProjectileFunctionLibrary::SpawnOxygenBubble(const UObject* WorldContextObject, const FVector& pSource, class APlayerCharacter* pTarget)
{
	const auto world = WorldContextObject->GetWorld();
	AProjectileActorManager::SpawnOxygenBubble(world, pSource, pTarget);
}
