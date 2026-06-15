// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Committed.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


UCommitted::UCommitted() {
	TypeId = EEnchantmentTypeID::Committed;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return 0.25f + level * 0.25f;
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UCommitted::CalculateEffects(const UHealthComponent* healthComponent, float& outDamageMultiplier, FGameplayTag& gameplayTag) const {

	const auto healthPercentage = healthComponent->GetCurrentHealth() / healthComponent->GetMaximumHealth();
	outDamageMultiplier = 1.0f + ((1.0f - healthPercentage) * (IsOwnerMob() ? MobMaxDamageBonus : LevelMultiplier(Level)));

	const float normalizedMagnitude = outDamageMultiplier / 2.f;

	if (normalizedMagnitude < 0.33f) {
		gameplayTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Committed");
	}
	else if (normalizedMagnitude < 0.66f) {
		gameplayTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Committed.Medium");
	}
	else {
		gameplayTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Committed.Strong");
	}
}

void UCommitted::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if(!IsSourceItemMelee()) {
		return;
	}
	const auto characterOwner = GetCharacterOwner();
	if (const auto target = Cast<ABaseCharacter>(targetComponent->GetAvatarActor())) {
		if (const auto healthComponent = target->FindComponentByClass<UHealthComponent>()) {

			float damageMultiplier;
			FGameplayTag gameplayTag;
			CalculateEffects(healthComponent, damageMultiplier, gameplayTag);

			effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(gameplayTag);
			
			mutableSpec.SetSetByCallerMagnitude(effects::HealthName, mutableSpec.GetSetByCallerMagnitude(effects::HealthName) * damageMultiplier);
			mutableSpec.DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Melee"));
		}
	}
}

void UCommitted::OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	if ((randStream.FRandRange(0.0f, 1.0f) < LevelMultiplier(Level)) || bAlwaysTrigger) {
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();

		if (auto target = Cast<ABaseCharacter>(toWhom)) {
			if (const auto healthComponent = target->FindComponentByClass<UHealthComponent>()) {

				float damageMultiplier;
				FGameplayTag gameplayTag;
				CalculateEffects(healthComponent, damageMultiplier, gameplayTag);

				outDamage *= damageMultiplier;

				auto abilitySystem = Cast<ABaseCharacter>(toWhat)->GetAbilitySystemComponent();
				FGameplayCueParameters params;
				params.Location = toWhat->GetActorLocation();
				params.Instigator = GetOwner();
				abilitySystem->ExecuteGameplayCue(gameplayTag, params);
			}
		}
	}
}
