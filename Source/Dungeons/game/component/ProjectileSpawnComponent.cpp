


#include "ProjectileSpawnComponent.h"
#include "DungeonsGameMode.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/ProjectileManager.h"

// Sets default values for this component's properties
UProjectileSpawnComponent::UProjectileSpawnComponent()
{
	bNetAddressable = true;
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
}

void UProjectileSpawnComponent::InternalSpawnProjectile(const FVector Direction, const FVector Offset, int Count) {
	const FProjectileSpawnDefinition& SpawnDefinition = CurrentAttack.GetValue();
	auto owner = GetOwner();
	FVector Origin = owner->GetActorLocation();
	FRotator rotation = Direction.ToOrientationRotator();
	rotation.Yaw += Count * SpawnDefinition.AngleSpan / SpawnDefinition.NumberOfProjectiles;

	FTransform transform(rotation, rotation.RotateVector(Offset) + Origin);
	if (auto projectile = AProjectileActorManager::Pop_Projectile(SpawnDefinition.Projectile, transform, GetWorld())) {
		projectile->bCanTriggerEnchantments = true;
		projectile->LaunchProjectile(owner);
		projectile->EnableProjectileCollisions();
	}

	if (++Count == SpawnDefinition.NumberOfProjectiles) {
		CurrentAttackTimerHandle.Invalidate();
		CurrentAttack.Reset();
	} else if (SpawnDefinition.DelaySecondsPerProjectile > 0.f) {
		auto& timerManager = owner->GetWorld()->GetTimerManager();
		timerManager.SetTimer(CurrentAttackTimerHandle, FTimerDelegate::CreateUObject(this, &UProjectileSpawnComponent::InternalSpawnProjectile, Direction, Offset, Count), SpawnDefinition.DelaySecondsPerProjectile, false);
	}
}

void UProjectileSpawnComponent::MulticastSpawnProjectiles_Implementation(const FProjectileSpawnDefinition& SpawnDefinition, FVector Direction, FVector Offset) {
	//A client might for whatever reason still be running an attack when a new one comes along. Probably not a great experience, but this is a quick and dirty fix.
	if (CurrentAttackTimerHandle.IsValid()) {
		GetOwner()->GetWorldTimerManager().ClearTimer(CurrentAttackTimerHandle);
	}
	
	int i = 0;
	CurrentAttack = SpawnDefinition;
	do {
		InternalSpawnProjectile(Direction, Offset, i++);
	} while (SpawnDefinition.DelaySecondsPerProjectile <= 0.f && i < SpawnDefinition.NumberOfProjectiles);
}

void UProjectileSpawnComponent::SpawnProjectiles(const FProjectileSpawnDefinition& SpawnDefinition, FVector Direction, FVector Offset) {
	//Dirty server side check to make sure we do not spawn projectiles while a definition is currently running...
	if (GetOwnerRole() == ROLE_Authority && !CurrentAttack.IsSet()) {
		MulticastSpawnProjectiles(SpawnDefinition, Direction, Offset);
	}
}
