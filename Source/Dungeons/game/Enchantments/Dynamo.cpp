// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "Dynamo.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/util/ValueFormat.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include <GameplayTagContainer.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

const FName UDynamoGameplayEffect::DamageIncreaseKey("DamageIncrease");

UDynamoGameplayEffect::UDynamoGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 20;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo"));
}

UDynamoMeleeGameplayEffect::UDynamoMeleeGameplayEffect() {
	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageIncreaseAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = UDynamoGameplayEffect::DamageIncreaseKey;

	damageInfo.ModifierMagnitude = dealDamageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo.Melee"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo.Melee"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Dynamo.Melee"),1,1);
	GameplayCues.Last().MagnitudeAttribute = damageInfo.Attribute;
}

UDynamoRangedGameplayEffect::UDynamoRangedGameplayEffect() {
	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = URangedAttributeSet::RangedAttackDamageIncreaseAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = UDynamoGameplayEffect::DamageIncreaseKey;

	damageInfo.ModifierMagnitude = dealDamageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo.Ranged"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo.Ranged"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Dynamo.Ranged"), 1, 1);
	GameplayCues.Last().MagnitudeAttribute = damageInfo.Attribute;
}

UDynamo::UDynamo() {
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseDamage + (level - 1) * DamageIncreasePerLevel;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void UDynamo::Consume() {
	FGameplayEffectQuery query;
	query.EffectDefinition = Effect;
	GetAbilitySystemComponent()->RemoveActiveEffects(query);
	CurrentStackCount = 0;
}

void UDynamo::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	Consume();
}

void UDynamo::OnDodgeRollEnd(FPredictionKey key) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(UDynamoGameplayEffect::DamageIncreaseKey, LevelMultiplier(Level));
	auto handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);
	CurrentStackCount = abilitySystem->GetCurrentStackCount(handle);
}

UDynamoMelee::UDynamoMelee() {
	TypeId = EEnchantmentTypeID::DynamoMelee;
	Effect = UDynamoMeleeGameplayEffect::StaticClass();
}

void UDynamoMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (const int stackCount = StackCount()) {
		if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(toWhom)) {
			FGameplayCueParameters params;
			params.RawMagnitude = stackCount;
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Dynamo.Melee"), params);
		}
	}
}

void UDynamoMelee::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	if ((hitCount > 0) && (GetOwnerRole() == ROLE_Authority)) Consume();
}


UDynamoRanged::UDynamoRanged() {
	TypeId = EEnchantmentTypeID::DynamoRanged;
	Effect = UDynamoRangedGameplayEffect::StaticClass();
}

void UDynamoRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream)
{
	if (GetOwnerRole() == ROLE_Authority) {
		Consume();
	}
}

const FName UDynamoRanged::DynamoRangedStackCountKey(TEXT("DynamoStackCount"));
