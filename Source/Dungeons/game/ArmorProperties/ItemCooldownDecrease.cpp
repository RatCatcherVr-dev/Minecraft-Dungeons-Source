#include "Dungeons.h"
#include "ItemCooldownDecrease.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/ItemAttributeSet.h"


void UItemCooldownDecrease::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto character = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = character->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UItemCooldownDecreaseGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(itemcooldowneffect::DataName, 1.0f/Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UItemCooldownDecrease::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;

	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UItemCooldownDecrease::UItemCooldownDecrease() {
	TypeID = EArmorPropertyID::ItemCooldownDecrease;
}

UItemCooldownDecreaseGameplayEffect::UItemCooldownDecreaseGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	cooldownMagnitude.DataName = itemcooldowneffect::DataName;

	cooldownInfo.ModifierMagnitude = cooldownMagnitude;
	cooldownInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(cooldownInfo);
}


