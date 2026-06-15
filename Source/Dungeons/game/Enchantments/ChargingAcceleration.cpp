#include "Dungeons.h"

#include "ChargingAcceleration.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/ui/DungeonsGameplayEffectUIData.h"
#include "game/component/RangedAttackComponent.h"

namespace chargeacceleration {
	static const int MaxStackCount = 40;
}

UChargingAcceleration::UChargingAcceleration() {
	TypeId = EEnchantmentTypeID::ChargingAcceleration;
	ServerOnlyExecution = true;

	LevelMultiplier = [](int level) { return 1.f; };
	MultiplierFormatter = valueformat::asConstant;
}

void UChargingAcceleration::OnStart() {
	Super::OnStart();

	if (GetOwner()->HasAuthority()) {
		if (auto rangedAttack = GetOwner()->FindComponentByClass<URangedAttackComponent>()) {
			rangedAttack->OnDurationPassed.AddUObject(this, &UChargingAcceleration::OnAttackOver);
		}
		GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UChargingAcceleration::OnCharge, ChargeIntervalSeconds, true);
	}
}

void UChargingAcceleration::OnEnd() {
	Super::OnEnd();
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(AccelerationEffectHandle);
	abilitySystem->RemoveActiveGameplayEffect(ChargeEffectHandle);
}

void UChargingAcceleration::OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key) {
	if (GetOwner()->HasAuthority()) {
		bInAttack = true;
	}
}

void UChargingAcceleration::OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwner()->HasAuthority()) {
		UnCharge();
	}
}

//// jryden: disabled for compile-OK after rebase, ask niklas/BarneyBear about surfacing UI data
//float UChargingAcceleration::GetCurrentCounterValue() {
//	const auto scale = 100.f / chargeacceleration::MaxStackCount;
//	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
//		if (auto active = abilitySystem->GetActiveGameplayEffect(AccelerationEffectHandle)) {
//			return active->Spec.StackCount * scale;
//		}
//	}
//	return 0;
//}
//
//float UChargingAcceleration::GetMaxCounterValue() {
//	return 100.f;
//}

void UChargingAcceleration::OnCharge() {
	if (!bInAttack) {
		auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		auto spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect);
		spec.SetSetByCallerMagnitude(TEXT("ChargeIncrease"), AccelerationAmount);
		// jryden: disabled for compile-OK after rebase
		//Cast<UDungeonsGameplayEffectUIData>(spec.Def->UIData)->AssociatedEnchantment = this;
		AccelerationEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		//BroadcastEnchantmentCurrentCounterValueChangedEvent();

		if (abilitySystem->GetActiveGameplayEffect(AccelerationEffectHandle)->Spec.StackCount >= chargeacceleration::MaxStackCount) {
			FGameplayCueParameters params;
			params.Location = GetOwner()->GetActorLocation();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.ChargingAcceleration.Full"), params);
			abilitySystem->RemoveActiveGameplayEffect(ChargeEffectHandle);
			GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);
		}
		else {
			auto chargeSpec = effects::CreateGameplayEffectSpec<UChargingAccelerationChargeGameplayEffect>(abilitySystem);
			ChargeEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(chargeSpec);
		}
	}
}

void UChargingAcceleration::UnCharge() {
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(AccelerationEffectHandle, 1);
	abilitySystem->RemoveActiveGameplayEffect(ChargeEffectHandle);
	// jryden: disabled for compile-OK after rebase
	//BroadcastEnchantmentCurrentCounterValueChangedEvent();
}

void UChargingAcceleration::OnAttackOver() {
	if (GetOwner()->HasAuthority()) {
		bInAttack = false;
		GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UChargingAcceleration::OnCharge, ChargeIntervalSeconds, true);
	}
}

UChargingAccelerationBaseGameplayEffect::UChargingAccelerationBaseGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = chargeacceleration::MaxStackCount;

	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Additive;
	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = TEXT("ChargeIncrease");
	rangedDamageInfo.ModifierMagnitude = speedMagnitude;

	Modifiers.Add(rangedDamageInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.ChargingAcceleration")), 0, 1);
}

UChargingAccelerationChargeGameplayEffect::UChargingAccelerationChargeGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = 1;
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.ChargingAcceleration.Charge")), 0, 1);
}