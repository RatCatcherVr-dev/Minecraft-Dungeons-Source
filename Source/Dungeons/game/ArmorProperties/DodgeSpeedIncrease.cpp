#include "Dungeons.h"
#include "DodgeSpeedIncrease.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"


namespace DodgeSpeed {
	FName DodgeSpeedName(TEXT("DodgeSpeed"));
}

UDodgeSpeedIncreaseGameplayEffect::UDodgeSpeedIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::DodgeSpeedAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = DodgeSpeed::DodgeSpeedName;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);
}

UDodgeSpeedIncrease::UDodgeSpeedIncrease() : Effect(UDodgeSpeedIncreaseGameplayEffect::StaticClass()) {
	TypeID = EArmorPropertyID::DodgeSpeedIncrease;
}

void UDodgeSpeedIncrease::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;


	auto character = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = character->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(DodgeSpeed::DodgeSpeedName, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UDodgeSpeedIncrease::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;

	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}