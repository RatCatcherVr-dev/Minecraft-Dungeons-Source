// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "EnigmaResonator.h"
#include "game/component/SoulComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


UEnigmaResonatorBase::UEnigmaResonatorBase() {
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return 0.1f + (0.05f * level);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

FText UEnigmaResonatorBase::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRoundedMultiple(DamageMultiplier)));
}


UEnigmaResonatorMelee::UEnigmaResonatorMelee() {
	TypeId = EEnchantmentTypeID::EnigmaResonatorMelee;
}

void UEnigmaResonatorMelee::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	float chance = 0;
	if (const auto soulComponent = GetOwner()->FindComponentByClass<USoulComponent>()) {
		chance = soulComponent->GetSoulPercentage() * LevelMultiplier(Level);
	}

	if ((randStream.FRandRange(0.0f, 1.0f) < chance) || bAlwaysTrigger) {
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		mutableSpec.SetSetByCallerMagnitude(effects::HealthName, mutableSpec.GetSetByCallerMagnitude(effects::HealthName) * DamageMultiplier);
		mutableSpec.DynamicAssetTags.AddTag(CriticalDamageType);
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Critical.Melee")));
	}
}

UEnigmaResonatorRanged::UEnigmaResonatorRanged() {
	TypeId = EEnchantmentTypeID::EnigmaResonatorRanged;
}

void UEnigmaResonatorRanged::OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	float chance = 0;
	if (const auto soulComponent = GetOwner()->FindComponentByClass<USoulComponent>()) {
		chance = soulComponent->GetSoulPercentage() * LevelMultiplier(Level);
	}

	if ((randStream.FRandRange(0.0f, 1.0f) < chance) || bAlwaysTrigger) {
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		outDamage *= DamageMultiplier;
		damageType = CriticalDamageType;

		auto abilitySystem = Cast<ABaseCharacter>(toWhat)->GetAbilitySystemComponent();
		FGameplayCueParameters params;
		params.Location = toWhat->GetActorLocation();
		params.Instigator = GetOwner();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Critical"), params);
	}
}
