#include "Dungeons.h"
#include "BowsBoon.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/RangedAttributeSet.h"

namespace {
	FName BowsBoonAttackBoostEffectMagnitude(TEXT("BownBoonAttackBoostEffectMagnitude"));
	FName BowsBoonDurationEffectMagnitude(TEXT("BownBoonDurationEffectMagnitude"));
}

UBowsBoon::UBowsBoon() {
	Effect = UBowsBoonGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::BowsBoon;
}

void UBowsBoon::OnProjectileOverlap(AActor *what, AActor *whom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	if (auto targetCharacter = Cast<ABaseCharacter>(what)) {
		auto characterOwner = GetCharacterOwner();
		if (targetCharacter == characterOwner) return;

		BroadcastEnchantmentTriggeredEvent();

		UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();

		FGameplayEffectSpec spec(Cast<UBowsBoonGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude(::BowsBoonAttackBoostEffectMagnitude, 1.3f);
		spec.SetSetByCallerMagnitude(::BowsBoonDurationEffectMagnitude, 3.0f + (float)Level);

		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetCharacter->GetAbilitySystemComponent());
	}
}

UBowsBoonGameplayEffect::UBowsBoonGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = ::BowsBoonDurationEffectMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat damageMagnitude;
	damageMagnitude.DataName = ::BowsBoonAttackBoostEffectMagnitude;

	damageInfo.ModifierMagnitude = damageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(damageInfo);

	//GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.BowsBoon"), 0, 1);
}