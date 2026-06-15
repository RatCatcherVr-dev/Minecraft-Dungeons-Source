
// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Regeneration.h"
#include "game/component/HealthComponent.h"
#include "game/util/ValueFormat.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"

const FName UHealthPerSecondCalculation::HealthPerSecondName = TEXT("HealthPerSecond");
UHealthPerSecondCalculation::UHealthPerSecondCalculation(){}

float UHealthPerSecondCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	return Spec.GetSetByCallerMagnitude(HealthPerSecondName) * Spec.Period;
}


URegeneration::URegeneration() {
	TypeId = EEnchantmentTypeID::Regeneration;
	Effect = URegenerationGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return RegenerationAmountPerSecond + ((RegenerationAmountPerSecond * level) / 3.0f);
	};
	MultiplierFormatter = valueformat::asSingleDecimalDamage;
}

FText URegeneration::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedSecond(TimeUntilRegeneration)));
}

void URegeneration::OnStart() {
	Super::OnStart();
	bIsPlayerOwner = GetOwner()->IsA(APlayerCharacter::StaticClass()) ? true : false;
	ResetTimer();
}

void URegeneration::OnEnd() {
	Super::OnEnd();
	StopRegeneration();
}

void URegeneration::StopRegeneration() {
	const auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	if (effectHandle.IsValid() && abilitySystem->GetActiveGameplayEffect(effectHandle)) {
		abilitySystem->RemoveActiveGameplayEffect(effectHandle);
	}
}

void URegeneration::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {	
	ResetTimer();
	StopRegeneration();
}

void URegeneration::OnStartRegeneration() {
	auto characterOwner = GetCharacterOwner();

	if (const auto healthComponent = characterOwner->FindComponentByClass<UHealthComponent>()) {
		if ((healthComponent->GetCurrentHealth() < healthComponent->GetMaximumHealth())) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
			const auto abilitySystem = characterOwner->GetAbilitySystemComponent();
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<URegenerationGameplayEffect>(abilitySystem, Level);
			const float ScaledRegenerationAmountPerSecond = bIsPlayerOwner ? LevelMultiplier(Level) : MobRegenerationAmountPerSecond;			
			spec.SetSetByCallerMagnitude(effects::HealthName, ScaledRegenerationAmountPerSecond);
			effectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

			BroadcastEnchantmentTriggeredEvent();
		}
	}
}

void URegeneration::ResetTimer() {
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &URegeneration::OnStartRegeneration, IsOwnerMob() ? MobTimeUntilRegeneration : TimeUntilRegeneration);
}

URegenerationGameplayEffect::URegenerationGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	Period = 0.25f;

	FCustomCalculationBasedFloat healthPerSecondMagnitude;
	healthPerSecondMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModHealingCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthPerSecondMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.Regeneration")), 0, 1);
}



