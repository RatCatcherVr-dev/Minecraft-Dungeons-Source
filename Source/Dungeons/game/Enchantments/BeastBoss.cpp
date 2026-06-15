#include "Dungeons.h"
#include "BeastBoss.h"

#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName BeastBossBonusDamageMeleeDamageEffectMagnitude(TEXT("BeastBossBonusDamageMeleeDamageEffectMagnitude"));
	FName BeastBossBonusDamageRangedDamageEffectMagnitude(TEXT("BeastBossBonusDamageRangedDamageEffectMagnitude"));
}

UBeastBossDamageBoostGameplayEffect::UBeastBossDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo meleeDamageInfo;
	meleeDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat meleeDamageMagnitude;
	meleeDamageMagnitude.DataName = BeastBossBonusDamageMeleeDamageEffectMagnitude;

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = BeastBossBonusDamageRangedDamageEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BeastBossBonusDamage"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BeastBossBonusDamage"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.BeastBossBonusDamage"), 0, 1);
}

UBeastBoss::UBeastBoss() {
	TypeId = EEnchantmentTypeID::BeastBoss;
	Effect = UBeastBossDamageBoostGameplayEffect::StaticClass();
	LevelMultiplier = [this](int level) -> float {
		return BaseDamageBoost + ((level - 1) * PerLevelDamageBoost);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UBeastBoss::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto owner = GetCharacterOwner();
	owner->OnMinionAcquired.AddUObject(this, &UBeastBoss::ApplyBuffToMinion);
	owner->OnMinionReleased.AddUObject(this, &UBeastBoss::RemoveBuffFromMinion);

	// and apply to existing minions here
	for (auto minion : owner->GetMinions())	{
		ApplyBuffToMinion(minion);
	}
}

void UBeastBoss::ApplyBuffToMinion(ABaseCharacter* minion) {
	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), Level);
	auto levelMultiplier = 1.0 + LevelMultiplier(Level);
	spec.SetSetByCallerMagnitude(::BeastBossBonusDamageMeleeDamageEffectMagnitude, levelMultiplier);
	spec.SetSetByCallerMagnitude(::BeastBossBonusDamageRangedDamageEffectMagnitude, levelMultiplier);

	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, minion->GetAbilitySystemComponent());
}

void UBeastBoss::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto owner = GetCharacterOwner();
	for (auto minion : owner->GetMinions())	{
		RemoveBuffFromMinion(minion);
	}
}

void UBeastBoss::RemoveBuffFromMinion(ABaseCharacter* minion)	{
	minion->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(
		FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.BeastBossBonusDamage")));
}
