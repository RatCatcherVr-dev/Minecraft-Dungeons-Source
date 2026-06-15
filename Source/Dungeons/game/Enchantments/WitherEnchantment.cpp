#include "WitherEnchantment.h"
#include "Dungeons.h"
#include "game/abilities/effects/WitherGameplayEffect.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/WitherPoisonVisualizationComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UWitherEnchantment::UWitherEnchantment() {
	ApplierEffect = UWitherApplierGameplayEffect::StaticClass();
}

void UWitherEnchantment::ApplyWitherPoison(ABaseCharacter* character) {
	if (!actorquery::isAlive(character)) {
		return;
	}
	if (auto witherDamage = character->FindComponentByClass<UWitherPoisonVisualizationComponent>()) {
		witherDamage->Activate();
	}
	auto owner = GetCharacterOwner();
	auto ownerAbilitySystem = owner->GetAbilitySystemComponent();
	auto targetAbilitySystem = character->GetAbilitySystemComponent();
	auto spec = effects::CreateGameplayEffectSpecFromSubClass(ownerAbilitySystem, ApplierEffect);
	spec.SetSetByCallerMagnitude(effects::DurationName, wither::getWitherDurationForFraction(WitherDamageTotalFraction));
	spec.GetContext().AddInstigator(owner, owner);
	ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
}

UWitherEnchantmentMelee::UWitherEnchantmentMelee() {
	TypeId = EEnchantmentTypeID::WitherEnchantmentMelee;
}

void UWitherEnchantmentMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	if (GetOwner()->HasAuthority()) {
		if (auto character = Cast<ABaseCharacter>(toWhom)) {
			ApplyWitherPoison(character);
		}
	}
}

UWitherEnchantmentRanged::UWitherEnchantmentRanged() {
	TypeId = EEnchantmentTypeID::WitherEnchantmentRanged;
}

void UWitherEnchantmentRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		if (auto character = Cast<ABaseCharacter>(toWhom)) {
			ApplyWitherPoison(character);
		}
	}
}
