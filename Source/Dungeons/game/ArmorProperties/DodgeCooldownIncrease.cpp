#include "Dungeons.h"
#include "DodgeCooldownIncrease.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"


namespace DodgeCooldown {
	FName DodgeCooldownName(TEXT("DodgeCooldown"));
}

UDodgeCooldownIncreaseGameplayEffect::UDodgeCooldownIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo cooldownInfo;
	cooldownInfo.Attribute = UMovementAttributeSet::DodgeCooldownAttribute();

	FSetByCallerFloat cooldownMagnitude;
	cooldownMagnitude.DataName = DodgeCooldown::DodgeCooldownName;

	cooldownInfo.ModifierMagnitude = cooldownMagnitude;
	cooldownInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(cooldownInfo);
}

UDodgeCooldownIncrease::UDodgeCooldownIncrease() : Effect(UDodgeCooldownIncreaseGameplayEffect::StaticClass()) {
	TypeID = EArmorPropertyID::DodgeCooldownIncrease;
}

void UDodgeCooldownIncrease::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;


	auto character = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = character->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(DodgeCooldown::DodgeCooldownName, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UDodgeCooldownIncrease::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;

	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}