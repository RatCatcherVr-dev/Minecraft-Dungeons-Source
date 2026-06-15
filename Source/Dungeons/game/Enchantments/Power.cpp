#include "Dungeons.h"
#include "Power.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/RangedAttributeSet.h"

namespace {
	FName PowerDealDamageEffectMagnitude(TEXT("PowerDealDamageMagnitude"));
}

UPower::UPower() {
	Effect = UPowerGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Power;

	LevelMultiplier = [this](int level) -> float {
		return FMath::Pow(1.1f, level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UPower::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UPowerGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(::PowerDealDamageEffectMagnitude, IsOwnerMob() ? MobDamageMultiplier : LevelMultiplier(Level));
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UPower::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UPowerGameplayEffect::UPowerGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::PowerDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}