

#include "PotionCooldownDecrease.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"


namespace PotionCooldownDecrease {
	FName PotionCooldownDecreaseName(TEXT("PotionCooldownDecrease"));
}

UPotionCooldownDecreaseGameplayEffect::UPotionCooldownDecreaseGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	FSetByCallerFloat MultiplierChange;
	MultiplierChange.DataName = PotionCooldownDecrease::PotionCooldownDecreaseName;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo cooldownInfo;
	cooldownInfo.Attribute = UItemAttributeSet::PotionCooldownMultiplierAttribute();
	cooldownInfo.ModifierMagnitude = MultiplierChange;
	cooldownInfo.ModifierOp = EGameplayModOp::Multiplicitive;

	Modifiers.Add(cooldownInfo);
}


UPotionCooldownDecrease::UPotionCooldownDecrease() : EffectToApply(UPotionCooldownDecreaseGameplayEffect::StaticClass()){
	TypeID = EArmorPropertyID::PotionCooldownDecrease;
}


void UPotionCooldownDecrease::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto ownerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		ensure(ownerAbilitySystem);
		
		FGameplayEffectSpec spec(EffectToApply.GetDefaultObject(), ownerAbilitySystem->MakeEffectContext(), 1.f);
		spec.SetSetByCallerMagnitude(PotionCooldownDecrease::PotionCooldownDecreaseName, CooldownDecrease);
		Handle = ownerAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UPotionCooldownDecrease::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	if (GetOwnerRole() == ROLE_Authority) {
		auto ownerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		ownerAbilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
