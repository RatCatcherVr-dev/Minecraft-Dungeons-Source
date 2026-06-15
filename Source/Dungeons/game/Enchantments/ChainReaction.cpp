#include "Dungeons.h"
#include "ChainReaction.h"
#include "game/actor/item/Arrow.h"
#include "DungeonsGameMode.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/component/RangedAttackComponent.h"


UChainReaction::UChainReaction() {
	TypeId = EEnchantmentTypeID::ChainReaction;
	LevelMultiplier = [this](int level) -> float {
		return 0.1f * level;
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}


FText UChainReaction::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asConstantWord(ArrowsToSpawn)));
}

bool UChainReaction::CanTrigger(const ABaseProjectile* projectile) {
	if (projectile->GetProjectileItemType() == game::item::type::TormentProjectile.getId()) {
		// This combination will create a massive amounts of arrows, and fixing it properly is surprisingly hard with the current code.
		return false;
	}
	return projectile->bCanTriggerEnchantments || (projectile->SpawnRecursionCounter < 2 && projectile->SpawnRecursionCounter > 0);
}

void UChainReaction::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	// TODO: fix issue w infinite recursion (well, until stack overflows)

	auto owner = GetOwner();
	if (!owner->HasAuthority())
		return;

	auto targetMob = Cast<ABaseCharacter>(toWhat);
	if (!targetMob) return;

	bool shouldAlwaysTrigger = bAlwaysTrigger && fromProjectile->SpawnRecursionCounter < 1;

	if (CanTrigger(fromProjectile) && (shouldAlwaysTrigger || (randStream.FRand() < LevelMultiplier(Level))))
	{
		BroadcastEnchantmentTriggeredEvent();
		auto characterOwner = GetCharacterOwner();
		
		

		const auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		const FVector &actorLocation = targetMob->GetActorLocation();
		FRotator rotation = fromProjectile->GetActorRotation();

		TSubclassOf<ABaseProjectile> ProjectileClass = fromProjectile->GetClass();

		const auto mobCapsule = targetMob->FindComponentByClass<UCapsuleComponent>();
		const auto mobCapsuleRadius = mobCapsule->GetScaledCapsuleRadius();

		const auto projectileSphere = fromProjectile->FindComponentByClass<USphereComponent>();
		const auto projectileSphereRadius = projectileSphere ? projectileSphere->GetScaledSphereRadius() : 10;

		const float fRotationSpawnOffset = ((mobCapsuleRadius + projectileSphereRadius) * 1.2f);
		const int iSpawnRecursionCount = fromProjectile->SpawnRecursionCounter + 1;
		const float DamageFactor = fromProjectile->GetDamageFactor() * 0.7f;

		SourceProjectile = fromProjectile;
		SpawnProjectilesMulticast(characterOwner, targetMob, ProjectileClass, actorLocation, rotation, fRotationSpawnOffset, DamageFactor, iSpawnRecursionCount );

	}
}

void UChainReaction::SpawnProjectilesMulticast_Implementation(ABaseCharacter* characterOwner, ABaseCharacter* targetMob, TSubclassOf<ABaseProjectile> ProjectileClass, FVector actorLocation, FRotator rotation, float fRotationSpawnOffset, float DamageFactor, int8 iSpawnRecursionCount)
{	
	float yawOffset = 360.0f / (float)ArrowsToSpawn;
	rotation.Pitch = 0.0f;

	auto gameMode = Cast<ADungeonsGameMode>(characterOwner->GetWorld()->GetAuthGameMode());
	auto rangedAttackComponent = characterOwner->FindComponentByClass<URangedAttackComponent>();

	if (!rangedAttackComponent) return;

	TOptional<float> SourceItemPower;
	float itemPower = 1.f;
	if (SourceProjectile) {
		SourceItemPower = SourceProjectile->SourceItemPower;
		itemPower = SourceProjectile->WeaponItemPower;
	}

	for (int i = 0; i < ArrowsToSpawn; ++i)
	{
		rotation.Yaw += yawOffset;

		const auto rotationVector = rotation.Vector();

		const auto spawnLocation = actorLocation + (rotationVector * fRotationSpawnOffset);
		FTransform transform(rotation, spawnLocation);

		if (auto projectile = URangedAttackComponent::SpawnProjectileDeferred(ProjectileClass, itemPower, transform, characterOwner, rangedAttackComponent->AttackDefinition(), false, false, SourceItemPower))
		{
			projectile->SetDamageFactor(DamageFactor);
			projectile->SpawnRecursionCounter = iSpawnRecursionCount;
			projectile->SetActorTransform(transform, false, nullptr, ETeleportType::ResetPhysics);

			projectile->LaunchProjectile(characterOwner);
			projectile->AddPreviouslyHitActor(targetMob);

			if (SourceProjectile) {
				projectile->SetGameplayEffectSpec(SourceProjectile->GetCachedOwnerGameplayEffectSpec().GetValue());
			}

			if (gameMode)
			{
				gameMode->OnActorSpawnedProjectile(GetOwner(), projectile);
			}

			projectile->EnableProjectileCollisions();

		}
	}

	SourceProjectile = nullptr;
}