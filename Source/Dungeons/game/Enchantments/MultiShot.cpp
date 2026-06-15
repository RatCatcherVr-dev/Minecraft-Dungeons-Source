#include "Dungeons.h"
#include "MultiShot.h"
#include "DungeonsGameMode.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "NoExportTypes.h"
#include <AbilitySystemComponent.h>
#include "game/actor/item/Arrow.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/component/RangedAttackComponent.h"

UMultiShot::UMultiShot() {
	TypeId = EEnchantmentTypeID::MultiShot;
	ServerOnlyExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return 0.1 + 0.1 * level;
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}


FText UMultiShot::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asConstantWord(ExtraArrowsWhenTriggered + 1)));
}

void UMultiShot::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> projectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwnerRole() != ROLE_Authority)
		return;

	if ((randStream.FRand() < LevelMultiplier(Level)) || bAlwaysTrigger) {
		BroadcastEnchantmentTriggeredEvent();
		auto characterOwner = GetCharacterOwner();

		SpawnProjectilesMulticast(characterOwner, attackTarget, attackTargetLocation, spawnLocation, rotation, projectileClass, projectile->WeaponItemPower, projectile->SourceItemPower.Get(-1.f), projectile->IsCharged, FMath::Rand());

	}
}

void UMultiShot::SpawnProjectilesMulticast_Implementation(ABaseCharacter* characterOwner, AActor* attackTarget, const FVector& attackTargetLocation, FVector spawnLocation, FRotator rotation, TSubclassOf<ABaseProjectile> projectileClass, float ItemPower, float sourceItemPower, bool isCharged,  int32 seed)
{	
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();	

	if (const auto rangedAttackComponent = characterOwner->FindComponentByClass<URangedAttackComponent>()) 
	{
		auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
		TOptional<float> SourceItemPower;
		if (sourceItemPower > 0) {
			SourceItemPower = sourceItemPower;
		}


		FTransform transform;
		transform.SetLocation(spawnLocation);

		bool clockwise = false;
		const float offsetAddition = 6.0f;
		float offset = offsetAddition;
		int updateOffsetCounter = 0;
		FRandomStream rnd(seed);
		for (int i = 0; i < ExtraArrowsWhenTriggered; ++i)
		{
			bool updateOffset = false;
			if (updateOffsetCounter >= 2)
			{
				updateOffset = true;
				updateOffsetCounter = 0;
				offset += offsetAddition;
			}
			updateOffsetCounter++;
			clockwise = !clockwise;

			FRotator newRotation = GetNewArrowRotation(rotation, clockwise, offset);

			transform.SetRotation(newRotation.Quaternion());

			if (auto newProjectile = URangedAttackComponent::SpawnProjectileDeferred(projectileClass, ItemPower, transform, characterOwner, rangedAttackComponent->AttackDefinition(), isCharged, false, SourceItemPower))
			{
				newProjectile->MultiplyDamageFactor(0.7f);
				newProjectile->IsHoming = false;
				newProjectile->LaunchProjectile(characterOwner);

				if (rangedAttackComponent->pitchMode == EPitchMode::Adjust) {
					URangedAttackComponent::TrySetDistanceBasedVelocity(newProjectile, transform.GetLocation(),  attackTarget ? attackTarget->GetActorLocation() : attackTargetLocation);
				}
				else if (rangedAttackComponent->pitchMode == EPitchMode::Random) {
					URangedAttackComponent::SetRandomizedPitchBasedVelocity(newProjectile, rangedAttackComponent->pitchRange, rnd);
				}

				if (gameMode)
				{
					gameMode->OnActorSpawnedProjectile(characterOwner, newProjectile);
				}

				newProjectile->EnableProjectileCollisions();
			}
		}
	}
}

FRotator UMultiShot::GetNewArrowRotation(const FRotator& baseRotation, bool clockwise, float offset) {
	FRotator newRotation = baseRotation;

	if (clockwise) {
		newRotation.Yaw += offset;
	}
	else {
		newRotation.Yaw -= offset;
	}

	return newRotation;
}
