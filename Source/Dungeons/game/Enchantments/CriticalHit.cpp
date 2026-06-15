#include "Dungeons.h"
#include "CriticalHit.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UCriticalHit::UCriticalHit() {
	TypeId = EEnchantmentTypeID::CriticalHit;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return 0.05f + (0.05f * level);
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

FText UCriticalHit::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRoundedMultiple(DamageMultiplier)));
}

void UCriticalHit::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	if ((randStream.FRandRange(0.0f, 1.0f) < LevelMultiplier(Level)) || bAlwaysTrigger) {
		if(GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		mutableSpec.SetSetByCallerMagnitude(effects::HealthName, mutableSpec.GetSetByCallerMagnitude(effects::HealthName) * DamageMultiplier);
		mutableSpec.DynamicAssetTags.AddTag(CriticalDamageType);
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.Critical.Melee")));
	}
}

void UCriticalHit::OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	if ((randStream.FRandRange(0.0f, 1.0f) < LevelMultiplier(Level)) || bAlwaysTrigger) {
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
