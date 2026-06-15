// Fill out your copyright notice in the Description page of Project Settings.
#include "DungeonsGearUtilLibrary.h"
#include "game/component/GearUtilComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "SubclassOf.h"
#include "GameplayEffectExtension.h"
#include "NoExportTypes.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "Dungeons.h"
#include "game/Enchantments/Enchantment.h"
#include "util/Algo.hpp"


void DungeonsGearUtilLibrary::OnBeforeMeleeDamageDealt(const ABaseCharacter* Character, float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, class UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom /*= nullptr*/, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeMeleeDamageDealt(outPushbackMultiplier, mutableSpec, targetComponent, overrideRandom, context);		
	}
}

void DungeonsGearUtilLibrary::OnAfterDealtMeleeDamage(const ABaseCharacter* Character, float damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream* overrideRandom /*= nullptr*/, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterDealtMeleeDamage(damage, missedAttack, toWhat, toWhom, fromLocation, atLocation, overrideRandom, context);	
	}
}

void DungeonsGearUtilLibrary::OnBeforeMeleeAttack(const ABaseCharacter* Character, AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeMeleeAttack(attackTarget, attackVector, index, overrideRandom, context);		
	}
}

void DungeonsGearUtilLibrary::OnAfterMeleeAttack(const ABaseCharacter* Character, int32 hitCount, AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterMeleeAttack(attackTarget, hitCount, attackVector, index, overrideRandom, context);
	}
}

void DungeonsGearUtilLibrary::OnBeforeAoeAttack(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeAoeAttack(attackTarget, overrideRandom, context);
	}
}

void DungeonsGearUtilLibrary::OnBeforeAoeAttackDamage(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeAoeAttackDamage(attackTarget, overrideRandom, context);		
	}
}

void DungeonsGearUtilLibrary::OnAfterAoeAttackDamage(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterAoeAttackDamage(attackTarget, overrideRandom, context);	
	}
}

void DungeonsGearUtilLibrary::OnProjectileOverlap(const ABaseCharacter* Character, AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, ABaseProjectile* fromProjectile) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnProjectileOverlap(what, whom, fromLocation, atLocation, fromProjectile);		
	}
}

void DungeonsGearUtilLibrary::OnProjectileLaunched(const ABaseCharacter* Character, ABaseProjectile* fromProjectile) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnProjectileLaunched(fromProjectile);		
	}
}

void DungeonsGearUtilLibrary::OnBeforeRangedDamageDealt(const ABaseCharacter* Character, float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeRangedDamageDealt(outDamage, damageType, fromProjectile, toWhat, toWhom, fromLocation, atLocation);		
	}
}

void DungeonsGearUtilLibrary::OnAfterDealtRangedDamage(const ABaseCharacter* Character, float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterDealtRangedDamage(Damage, fromProjectile, toWhat, toWhom, fromLocation, atLocation);	
	}
}

void DungeonsGearUtilLibrary::OnAfterRangedAttackPerProjectile(const ABaseCharacter* Character, AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key /*= FPredictionKey()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterRangedAttackPerProjectile(attackTarget, attackTargetLocation, projectile, ProjectileClass, spawnLocation, rotation, currentArrowCount, randStream, key);		
	}
}

void DungeonsGearUtilLibrary::OnAfterRangedAttack(const ABaseCharacter* Character, AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key /*= FPredictionKey()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterRangedAttack(attackTarget, ProjectileClass, charged, spawnLocation, rotation, currentArrowCount, randStream, key);		
	}
}

void DungeonsGearUtilLibrary::OnRangedAttackEnded(const ABaseCharacter* Character, bool completed, bool spawnedProjectiles) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnRangedAttackEnded(completed, spawnedProjectiles);		
	}	
}

void DungeonsGearUtilLibrary::OnBeforeRangedAttack(const ABaseCharacter* Character, AActor* attackTarget, bool& attackDenied, FPredictionKey key /*= FPredictionKey()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeRangedAttack(attackTarget, attackDenied, key);		
	}
}

void DungeonsGearUtilLibrary::OnAfterAmmoConsumed(const ABaseCharacter* Character, TSubclassOf<ABaseProjectile> projectileDefinition) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterAmmoConsumed(projectileDefinition);		
	}
}

void DungeonsGearUtilLibrary::OnResetRangedProjectile(const ABaseCharacter* Character, ABaseProjectile* projectile) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnResetRangedProjectile(projectile);		
	}
}

void DungeonsGearUtilLibrary::OnOverlappedByProjectile(const ABaseCharacter* Character, ABaseProjectile* projectile) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnOverlappedByProjectile(projectile);
	}
}

void DungeonsGearUtilLibrary::OnBeforeDamageReceived(const ABaseCharacter* Character, bool& outAttackMissed, FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom /*= nullptr*/, FPredictionKey key /*= FPredictionKey()*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnBeforeDamageReceived(outAttackMissed, data, targetComponent, overrideRandom, key);		
	}
}

void DungeonsGearUtilLibrary::OnAfterReceivedDamage(const ABaseCharacter* Character, const FGameplayEffectModCallbackData &data, FRandomStream* overrideRandom /*= nullptr*/) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterReceivedDamage(data, overrideRandom);		
	}
}

void DungeonsGearUtilLibrary::OnDodgeRollStart(const ABaseCharacter* Character, const FVector& dodgeDirection, FPredictionKey key) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnDodgeRollStart(dodgeDirection, key);		
	}
}

void DungeonsGearUtilLibrary::OnDodgeRollEnd(const ABaseCharacter* Character, FPredictionKey key) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnDodgeRollEnd(key);		
	}
}

bool DungeonsGearUtilLibrary::OnAfterDropRolled(const ABaseCharacter* Character, const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability) {
	bool handled = false;
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		handled = GearUtilComponent->OnAfterDropRolled(dropData, dropSource, probability) || handled;
	}
	return handled;
}

void DungeonsGearUtilLibrary::OnEmeraldsCollected(const ABaseCharacter* Character, int32 count) {	
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnEmeraldsCollected(count);
	}
}

void DungeonsGearUtilLibrary::OnAfterResurrection(const ABaseCharacter* Character) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnAfterResurrection();
	}
}

void DungeonsGearUtilLibrary::OnInvisibilityRemoved(const ABaseCharacter* Character) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnInvisibilityRemoved();
	}
}

void DungeonsGearUtilLibrary::OnItemActivated(const ABaseCharacter * Character, const AItemInstance * item, FPredictionKey key) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnItemActivated(item, key);
	}
}
TOptional<UGearUtil*> DungeonsGearUtilLibrary::ChangeItemRarity(const ABaseCharacter* Character, EItemRarity& rarity)
{
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		TOptional<UGearUtil*> GearUtil = GearUtilComponent->ChangeItemRarity(rarity);
		if (GearUtil.IsSet())
		{
			return GearUtil;
		}
	}
	return TOptional<UGearUtil*>();
}

void DungeonsGearUtilLibrary::OnInvisibilityGained(const ABaseCharacter* Character) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(Character)) {
		GearUtilComponent->OnInvisibilityGained();
	}
}

void DungeonsGearUtilLibrary::OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle) {
	for (UGearUtilComponent* GearUtilComponent : GetGearUtilComponents(player->GetControlledPlayerCharacter())) {
		GearUtilComponent->OnPlayerIdleChange(player, idle);
	}
}

TArray<UGearUtilComponent*> DungeonsGearUtilLibrary::GetGearUtilComponents(const ABaseCharacter* Character) {
	TArray<UGearUtilComponent*> comps = {};
	if (Character) {		
		Character->GetComponents<UGearUtilComponent>(comps);		
	}
	return comps;
}
