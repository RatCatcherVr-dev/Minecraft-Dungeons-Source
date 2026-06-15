#include "Dungeons.h"
#include "Punch.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/RangedAttributeSet.h"

namespace {
	FName PunchPushbackEffectMagnitude(TEXT("PunchPushbackEffectMagnitude"));
}

UPunch::UPunch() {
	Effect = UPunchGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Punch;

	LevelMultiplier = [this](int level) -> float {
		return 2.0f + level;
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UPunch::OnStart() {
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UPunchGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(::PunchPushbackEffectMagnitude, LevelMultiplier(Level));
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UPunch::OnEnd() {
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}


UPunchGameplayEffect::UPunchGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo pushbackInfo;
	pushbackInfo.Attribute = URangedAttributeSet::RangedAttackPushbackMultiplierAttribute();

	FSetByCallerFloat pushbackMagnitude;
	pushbackMagnitude.DataName = ::PunchPushbackEffectMagnitude;

	pushbackInfo.ModifierMagnitude = pushbackMagnitude;
	pushbackInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(pushbackInfo);
}



