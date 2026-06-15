// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiCharge.h"
#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"

namespace {
	FName MultiChargeDamageMultiplierEffectMagnitude(TEXT("MultiChargeDamageMultiplierEffectMagnitude"));
}

UMultiCharge::UMultiCharge() {
	TypeId = EEnchantmentTypeID::MultiCharge;

	EffectMultiplier = [this](int level) -> float {
		return 2.5f + (2.5f * (level - 1) + FMath::Clamp(0.5f * (level - 2), 0.f, 0.5f));
	};
	LevelMultiplier = [this](int level) -> float {
		return ++level;
	};
	MultiplierFormatter = valueformat::asConstant;

	ChargedLevelOneEffect = UMultiChargeChargedEffectOne::StaticClass();
	ChargedLevelTwoEffect = UMultiChargeChargedEffectTwo::StaticClass();
	ChargedLevelThreeEffect = UMultiChargeChargedEffectThree::StaticClass();
	ChargingEffect = UMultiChargeChargingEffect::StaticClass();
	DamageEffect = UMultiChargeDamageEffect::StaticClass();
}

void UMultiCharge::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority) {
		return;
	}

	AbilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	RangedComponent = Cast<URangedAttackComponent>(GetOwner()->GetComponentByClass(URangedAttackComponent::StaticClass()));
	ChargeHandle = RangedComponent->OnChargeComplete.AddUObject(this, &UMultiCharge::OnChargeComplete);
	StopHandle = RangedComponent->OnAttackStopped.AddUObject(this, &UMultiCharge::OnAttackStopped);
	if (RangedComponent->HasRangedWeaponEquipped()) {
		ChargeShootEffectTag = RangedComponent->GetCurrentWeapon()->GetChargeShootEffectTag();
	}
	CurrentLevel = 0;
}

void UMultiCharge::OnEnd() {
	Super::OnEnd();
	OnAttackStopped();

	RangedComponent->OnChargeComplete.Remove(ChargeHandle);
	ChargeHandle.Reset();

	RangedComponent->OnAttackStopped.Remove(StopHandle);
	StopHandle.Reset();
}

void UMultiCharge::OnChargeComplete(float chargeTime) {
	ChargeTime = chargeTime;

	FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	timerManager.ClearTimer(ChargeTimer);

	CurrentLevel = 0;
	AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer{ FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge") });

	DamageEffectHandle.Invalidate();

	TSubclassOf<UGameplayEffect> chargingEffect = ChargingEffect;
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
	FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(AbilitySystem, chargingEffect, Level));
	AbilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	timerManager.SetTimer(ChargeTimer, this, &UMultiCharge::OnMultiChargeComplete, ChargeTime);
}

void UMultiCharge::OnMultiChargeComplete() {
	FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	timerManager.ClearTimer(ChargeTimer);

	if (DamageEffectHandle.IsValid()) {
		AbilitySystem->RemoveActiveGameplayEffect(DamageEffectHandle);
		DamageEffectHandle.Invalidate();
	}

	CurrentLevel++;

	if (RangedComponent->HasRangedWeaponEquipped()) {
		switch (CurrentLevel) {
		case 1: RangedComponent->GetCurrentWeapon()->ChargeShootEffectTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.MultiCharge.Shot.One"); break;
		case 2: RangedComponent->GetCurrentWeapon()->ChargeShootEffectTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.MultiCharge.Shot.Two"); break;
		case 3: RangedComponent->GetCurrentWeapon()->ChargeShootEffectTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.MultiCharge.Shot.Three"); break;
		}
	}

	TSubclassOf<UGameplayEffect> chargedEffect;
	switch (Level) {
	case 1: chargedEffect = ChargedLevelOneEffect; break;
	case 2: chargedEffect = ChargedLevelTwoEffect; break;
	case 3: chargedEffect = ChargedLevelThreeEffect; break;
	}

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
	FGameplayEffectSpec multiChargeSpec(effects::CreateGameplayEffectSpecFromSubClass(AbilitySystem, chargedEffect, CurrentLevel));
	AbilitySystem->ApplyGameplayEffectSpecToSelf(multiChargeSpec);

	TSubclassOf<UGameplayEffect> damageEffect = DamageEffect;
	FGameplayEffectSpec damageSpec(effects::CreateGameplayEffectSpecFromSubClass(AbilitySystem, damageEffect, CurrentLevel));
	damageSpec.SetSetByCallerMagnitude(::MultiChargeDamageMultiplierEffectMagnitude, EffectMultiplier(CurrentLevel));
	DamageEffectHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(damageSpec);

	if (Level > CurrentLevel) {
		timerManager.SetTimer(ChargeTimer, this, &UMultiCharge::OnMultiChargeComplete, ChargeTime);
	}
}

void UMultiCharge::OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	OnAttackStopped();
}

void UMultiCharge::OnAttackStopped() {
	if (GetOwnerRole() != ROLE_Authority) {
		return;
	}

	FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	timerManager.ClearTimer(ChargeTimer);

	CurrentLevel = 0;
	AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer{ FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge") });

	DamageEffectHandle.Invalidate();
	if (RangedComponent->HasRangedWeaponEquipped()) {
		RangedComponent->GetCurrentWeapon()->ChargeShootEffectTag = ChargeShootEffectTag;
	}
}

UMultiChargeChargedEffect::UMultiChargeChargedEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.MultiCharge"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.MultiCharge.Charged"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
}

UMultiChargeChargedEffectOne::UMultiChargeChargedEffectOne(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	StackLimitCount = 1;
}

UMultiChargeChargedEffectTwo::UMultiChargeChargedEffectTwo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	StackLimitCount = 2;
}

UMultiChargeChargedEffectThree::UMultiChargeChargedEffectThree(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	StackLimitCount = 3;
}

UMultiChargeChargingEffect::UMultiChargeChargingEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.MultiCharge.Charging"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
}

UMultiChargeDamageEffect::UMultiChargeDamageEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::MultiChargeDamageMultiplierEffectMagnitude;

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();
	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);
	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiCharge"));
}