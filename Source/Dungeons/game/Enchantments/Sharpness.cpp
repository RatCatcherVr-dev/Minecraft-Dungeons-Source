#include "Dungeons.h"
#include "Sharpness.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"

namespace {
	FName SharpnessDealDamageEffectMagnitude(TEXT("SharpnessDealDamageMagnitude"));
}

USharpness::USharpness() {
	Effect = USharpnessGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Sharpness;
	LevelMultiplier = [this](int level) -> float {
		return FMath::Pow(1.1f, level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void USharpness::OnStart(){
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<USharpnessGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(::SharpnessDealDamageEffectMagnitude, LevelMultiplier(Level) );
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void USharpness::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle); 
}

USharpnessGameplayEffect::USharpnessGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::SharpnessDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}
