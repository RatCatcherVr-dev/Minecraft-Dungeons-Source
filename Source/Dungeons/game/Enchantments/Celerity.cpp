#include "Dungeons.h"
#include "Celerity.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName CelerityCooldownEffectMagnitude(TEXT("CelerityCooldownEffectMagnitude"));
}

UCelerity::UCelerity() {
	Effect = UCelerityGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Celerity;

	LevelMultiplier = [this](int level) -> float {
		return FMath::Pow(0.85f, 0.4f + static_cast<float>(level)*0.85f);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UCelerity::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;
	
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UCelerityGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(::CelerityCooldownEffectMagnitude, 1.0f / LevelMultiplier(Level));
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UCelerity::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority) return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UCelerityGameplayEffect::UCelerityGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo cooldownInfo;
	cooldownInfo.Attribute = UItemAttributeSet::ItemCooldownMultiplierAttribute();

	FSetByCallerFloat cooldownMagnitude;
	cooldownMagnitude.DataName = ::CelerityCooldownEffectMagnitude;

	cooldownInfo.ModifierMagnitude = cooldownMagnitude;
	cooldownInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(cooldownInfo);
}

