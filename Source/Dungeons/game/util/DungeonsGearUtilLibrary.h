// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoExportTypes.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "game/component/GearUtilComponent.h"
#include "game/GameSettings.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/drop/ItemDropGenerator.h"
/**
 * 
 */
class DungeonsGearUtilLibrary
{
public:
	static void OnBeforeMeleeDamageDealt(const ABaseCharacter* Character, float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, class UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom = nullptr, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnAfterDealtMeleeDamage(const ABaseCharacter* Character, float damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream* overrideRandom = nullptr, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnBeforeMeleeAttack(const ABaseCharacter* Character, AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnAfterMeleeAttack(const ABaseCharacter* Character, int32 hitCount, AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnBeforeAoeAttack(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnBeforeAoeAttackDamage(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnAfterAoeAttackDamage(const ABaseCharacter* Character, AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context = FSharedPredictionContext());
	static void OnProjectileOverlap(const ABaseCharacter* Character, AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, ABaseProjectile* fromProjectile);
	static void OnProjectileLaunched(const ABaseCharacter* Character, ABaseProjectile* fromProjectile);
	static void OnBeforeRangedDamageDealt(const ABaseCharacter* Character, float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation) ;
	static void OnAfterDealtRangedDamage(const ABaseCharacter* Character, float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation);
	static void OnAfterRangedAttackPerProjectile(const ABaseCharacter* Character, AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey());
	static void OnAfterRangedAttack(const ABaseCharacter* Character, AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey());
	static void OnRangedAttackEnded(const ABaseCharacter* Character, bool completed, bool spawnedProjectiles);
	static void OnBeforeRangedAttack(const ABaseCharacter* Character, AActor* attackTarget, bool& attackDenied, FPredictionKey key = FPredictionKey());
	static void OnAfterAmmoConsumed(const ABaseCharacter* Character, TSubclassOf<ABaseProjectile> projectileDefinition);
	static void OnResetRangedProjectile(const ABaseCharacter* Character, ABaseProjectile* projectile);
	static void OnOverlappedByProjectile(const ABaseCharacter* Character, ABaseProjectile* projectile);
	static void OnBeforeDamageReceived(const ABaseCharacter* Character, bool& outAttackMissed, FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom = nullptr, FPredictionKey key = FPredictionKey());
	static void OnAfterReceivedDamage(const ABaseCharacter* Character, const FGameplayEffectModCallbackData &data, FRandomStream* overrideRandom = nullptr);
	static void OnDodgeRollStart(const ABaseCharacter* Character, const FVector& dodgeDirection, FPredictionKey);
	static void OnDodgeRollEnd(const ABaseCharacter* Character, FPredictionKey);
	static bool OnAfterDropRolled(const ABaseCharacter* Character, const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability);
	static void OnEmeraldsCollected(const ABaseCharacter* Character, int32 count);
	static void OnAfterResurrection(const ABaseCharacter* Character);
	static void OnInvisibilityRemoved(const ABaseCharacter* Character);
	static void OnItemActivated(const ABaseCharacter* Character, const class AItemInstance* item, FPredictionKey key);
	static void OnInvisibilityGained(const ABaseCharacter* Character);
	static void OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle);



	static TOptional<UGearUtil*> ChangeItemRarity(const ABaseCharacter* Character, EItemRarity& rarity);
private:
	static TArray<UGearUtilComponent*> GetGearUtilComponents(const ABaseCharacter* Character);
};
