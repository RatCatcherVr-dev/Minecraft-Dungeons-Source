#include "GearUtilComponent.h"
#include "game/util/GearUtil.h"
#include "AbilitySystemComponent.h"
#include "game/actor/item/BaseProjectile.h"
#include "GameplayEffectExtension.h"
#include "game/ArmorProperties/ArmorPropertyType.h"


UGearUtilComponent::UGearUtilComponent()
{

}

void UGearUtilComponent::OnBeforeMeleeDamageDealt(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, class UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom /*= nullptr*/, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnBeforeDealtMeleeDamage(outPushbackMultiplier, mutableSpec, targetComponent, stream, localContext);
	}
}

void UGearUtilComponent::OnAfterDealtMeleeDamage(float damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream* overrideRandom /*= nullptr*/, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnAfterDealtMeleeDamage(damage, missedAttack, toWhat, toWhom, fromLocation, atLocation, stream, localContext);
	}
}

void UGearUtilComponent::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnBeforeMeleeAttack(attackTarget, attackVector, index, stream, localContext);
	}
}

void UGearUtilComponent::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnAfterMeleeAttack(attackTarget, hitCount, attackVector, index, stream, localContext);
	}
}

void UGearUtilComponent::OnBeforeAoeAttack(AActor * attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnBeforeAoeAttack(attackTarget, stream, localContext);
	}
}

void UGearUtilComponent::OnBeforeAoeAttackDamage(AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnBeforeDealtAoeDamage(attackTarget, stream, localContext);
	}
}

void UGearUtilComponent::OnAfterAoeAttackDamage(AActor* attackTarget, FRandomStream* overrideRandom, FSharedPredictionContext context /*= FSharedPredictionContext()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && overrideRandom && context.GetKey().IsValidKey() && GearUtil->CanExecuteOnClient();

		FRandomStream& stream = predictiveExecution ? *overrideRandom : DefaultRandom;
		FSharedPredictionContext localContext = predictiveExecution ? context : FSharedPredictionContext();
		//Force set current key here to stop using a prediction key if we are a non predictive GearUtil
		FUseSpecfiedKeyScopedPredictionWindow window(localContext);
		GearUtil->OnAfterDealtAoeDamage(attackTarget, stream, localContext);
	}
}

void UGearUtilComponent::OnProjectileOverlap(AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, ABaseProjectile* fromProjectile)
{
	for (auto GearUtil : GetValidGearUtils()) {
		if (GearUtil->ShouldTriggerOnProjectile(fromProjectile)) {
			GearUtil->OnProjectileOverlap(what, whom, fromLocation, atLocation, DefaultRandom);
		}
	}
}

void UGearUtilComponent::OnProjectileLaunched(ABaseProjectile* fromProjectile)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnProjectileLaunch(fromProjectile);
	}
}

void UGearUtilComponent::OnBeforeRangedDamageDealt(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation)
{
	for (auto GearUtil : GetValidGearUtils()) {
		if (GearUtil->ShouldTriggerOnProjectile(fromProjectile)) {
			GearUtil->OnBeforeDealtRangedDamage(outDamage, damageType, fromProjectile, toWhat, toWhom, fromLocation, atLocation, DefaultRandom);
		}
	}
}

void UGearUtilComponent::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation)
{
	for (auto GearUtil : GetValidGearUtils()) {
		if (GearUtil->ShouldTriggerOnProjectile(fromProjectile)) {
			GearUtil->OnAfterDealtRangedDamage(Damage, fromProjectile, toWhat, toWhom, fromLocation, atLocation, DefaultRandom);
		}
	}
}

void UGearUtilComponent::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key /*= FPredictionKey()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		const FRandomStream& stream = GearUtil->CanExecuteOnClient() ? randStream : DefaultRandom;
		GearUtil->OnAfterRangedAttackPerProjectile(attackTarget, attackLocation, projectile, ProjectileClass, spawnLocation, rotation, currentArrowCount, stream, key);
	}
}

void UGearUtilComponent::OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key /*= FPredictionKey()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		const FRandomStream& stream = GearUtil->CanExecuteOnClient() ? randStream : DefaultRandom;
		GearUtil->OnAfterRangedAttack(attackTarget, ProjectileClass, charged, spawnLocation, rotation, currentArrowCount, stream, key);
	}
}

void UGearUtilComponent::OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key /*= FPredictionKey()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnBeforeRangedAttack(attackTarget, attackDenied, key);
	}
}

void UGearUtilComponent::OnAfterAmmoConsumed(TSubclassOf<ABaseProjectile> projectileDefinition)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnAfterAmmoConsumed(projectileDefinition);
	}
}

void UGearUtilComponent::OnRangedAttackEnded(bool completed, bool didSpawnProjectiles) {
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnRangedAttackEnded(completed, didSpawnProjectiles);
	}
}

void UGearUtilComponent::OnResetRangedProjectile(ABaseProjectile* projectile)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnResetRangedProjectile(projectile);
	}
}

void UGearUtilComponent::OnOverlappedByProjectile(ABaseProjectile* projectile)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnOverlappedByProjectile(projectile, DefaultRandom);
	}
}

void UGearUtilComponent::OnBeforeDamageReceived(bool& outAttackMissed, FGameplayEffectModCallbackData& data, UAbilitySystemComponent* targetComponent, FRandomStream* overrideRandom /*= nullptr*/, FPredictionKey key /*= FPredictionKey()*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		if (GearUtil->IsPredictive() && overrideRandom && key.IsValidKey() && GearUtil->CanExecuteOnClient()) {
			GearUtil->OnBeforeReceivedDamage(outAttackMissed, data, targetComponent, *overrideRandom, key);
		}
		else if (GetOwnerRole() == ROLE_Authority) {
			GearUtil->OnBeforeReceivedDamage(outAttackMissed, data, targetComponent, DefaultRandom, FPredictionKey());
		}
	}
}

void UGearUtilComponent::OnAfterReceivedDamage(const FGameplayEffectModCallbackData &data, FRandomStream* overrideRandom /*= nullptr*/)
{
	for (auto GearUtil : GetValidGearUtils()) {
		if (GearUtil->IsPredictive() && overrideRandom && GearUtil->CanExecuteOnClient()) {
			GearUtil->OnAfterReceivedDamage(data, *overrideRandom);
		}
		else if (GetOwnerRole() == ROLE_Authority) {
			GearUtil->OnAfterReceivedDamage(data, DefaultRandom);
		}
	}
}

void UGearUtilComponent::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnDodgeRollStart(dodgeDirection, key);
	}
}

void UGearUtilComponent::OnDodgeRollEnd(FPredictionKey key)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnDodgeRollEnd(key);
	}
}

bool UGearUtilComponent::OnAfterDropRolled(const FDropCategoryDescription& dropData, const FItemDropSource& dropSource, float probability)
{
	auto dropped = false;

	for (auto GearUtil : GetValidGearUtils()) {
		const auto GearUtilDropped = GearUtil->OnAfterDropRolled(dropData, dropSource, probability);
		GearUtil->SetIsCurrentlyRolling(false);
		dropped = GearUtilDropped || dropped;
	}

	return dropped;
}

void UGearUtilComponent::OnEmeraldsCollected(int32 amount)
{
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnEmeraldsCollected(amount);
	}
}

void UGearUtilComponent::OnAfterResurrection() {
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnAfterResurrection();
	}
}

void UGearUtilComponent::OnInvisibilityRemoved() {
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnInvisibilityRemoved();
	}

}

void UGearUtilComponent::OnItemActivated(const AItemInstance * item, FPredictionKey key) {
	for (auto GearUtil : GetValidGearUtils()) {
		bool predictiveExecution = GearUtil->IsPredictive() && key.IsValidKey() && GearUtil->CanExecuteOnClient();

		GearUtil->OnItemActivated(item, key);
	}
}

void UGearUtilComponent::OnInvisibilityGained() {
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnInvisibilityGained();
	}
}

void UGearUtilComponent::OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle) {
	for (auto GearUtil : GetValidGearUtils()) {
		GearUtil->OnPlayerIdleChange(player, idle);
	}
}

TOptional<UGearUtil*> UGearUtilComponent::ChangeItemRarity(EItemRarity& rarity)
{
	for (auto GearUtil : GetValidGearUtils()) {
		TOptional<UGearUtil*> GearUtilOptional = GearUtil->ChangeItemRarity(rarity);
		if (GearUtilOptional.IsSet())
		{
			return GearUtilOptional;
		}
	}
	return TOptional<UGearUtil*>();
}
