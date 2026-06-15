#include "Dungeons.h"
#include "BeastSurge.h"

#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName BeastSurgeMeleeAttackSpeedEffectMagnitude(TEXT("BeastSurgeMeleeAttackSpeedEffectMagnitude"));
	FName BeastSurgeRangedAttackSpeedEffectMagnitude(TEXT("BeastSurgeRangedAttackSpeedEffectMagnitude"));
	FName BeastSurgeSpeedEffectMagnitude(TEXT("BeastSurgeSpeedEffectMagnitude"));
}


FGameplayModifierInfo CreateGameplayModifierInfo(FGameplayAttribute gameplayAttribute, FName dataName) {
	FGameplayModifierInfo gameplayModifier;
	gameplayModifier.Attribute = gameplayAttribute;

	FSetByCallerFloat gameplayDamageMagnitude;
	gameplayDamageMagnitude.DataName = dataName;

	gameplayModifier.ModifierMagnitude = gameplayDamageMagnitude;
	gameplayModifier.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	return gameplayModifier;
}

UBeastSurgeGameplayEffect::UBeastSurgeGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(10.0);

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	Modifiers.Add(CreateGameplayModifierInfo(
		UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute(),
		BeastSurgeMeleeAttackSpeedEffectMagnitude
		));

	Modifiers.Add(CreateGameplayModifierInfo(
		URangedAttributeSet::RangedAttackSpeedMultiplierAttribute(),
		BeastSurgeRangedAttackSpeedEffectMagnitude
	));

	Modifiers.Add(CreateGameplayModifierInfo(
		UMovementAttributeSet::SpeedMultiplierAttribute(),
		BeastSurgeSpeedEffectMagnitude
	));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BeastSurgeBonusDamage"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BeastSurgeBonusDamage"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.BeastSurgeBonusDamage"), 0, 1);
}

UBeastSurge::UBeastSurge() {
	TypeId = EEnchantmentTypeID::BeastSurge;
	Effect = UBeastSurgeGameplayEffect::StaticClass();
	LevelMultiplier = [this](int level) -> float {
		switch(level) {
		case 2:
			return Level2Modifier;
		case 3:
			return Level3Modifier;
		default:
			return BaseModifier;
		}
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

void UBeastSurge::OnHealthPotionUsed() {
	// create the effect to apply here
	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), Level);
	auto levelMultiplier = LevelMultiplier(Level);
	spec.SetSetByCallerMagnitude(BeastSurgeMeleeAttackSpeedEffectMagnitude, levelMultiplier);
	spec.SetSetByCallerMagnitude(BeastSurgeRangedAttackSpeedEffectMagnitude, levelMultiplier);
	spec.SetSetByCallerMagnitude(BeastSurgeSpeedEffectMagnitude, levelMultiplier);

	// and apply to the minions here
	auto owner = GetCharacterOwner();
	for (auto minion : owner->GetMinions())	{
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, minion->GetAbilitySystemComponent());
	}

	BroadcastEnchantmentTriggeredEvent();
}
