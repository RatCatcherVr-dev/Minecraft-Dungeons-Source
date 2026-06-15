#include "Dungeons.h"
#include "LightFeatherInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <AbilitySystemComponent.h>
#include "WindItemInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

ALightFeatherInstance::ALightFeatherInstance() {
	PowerEffects = { UStunDurationIncrease::StaticClass() };
	bManualPredictionHandling = true;
}

float ALightFeatherInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::StunDuration:
		return StunDurationSeconds;
	}
	return -1;
}

int ALightFeatherInstance::GetDisplayCount() const {
	return 0;
}

bool ALightFeatherInstance::CanActivate() const {
	const auto owner = Cast<ABaseCharacter>(GetOwner());
	ensure(owner);

	return Super::CanActivate() && !owner->IsImmobile() && !owner->GetCharacterMovement()->IsFalling();
}

void ALightFeatherInstance::OnPlayerLanded() {
	auto abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
	FScopedPredictionWindow(abilitySystem, CurrentPrediction);
	auto owner = Cast<APlayerCharacter>(GetOwner());

	if (onLandedHandle.IsValid()) {
		owner->OnCharacterWalking.Remove(onLandedHandle);
		onLandedHandle.Reset();
	}

	owner->OnExternalDodgeRollEnd(CurrentPrediction);
	ExecuteLightFeatherLandingGameplayCue(owner);
	ApplyStunEffectToMobs(owner);
	CurrentPrediction = FPredictionKey();
}

void ALightFeatherInstance::ExecuteLightFeatherLandingGameplayCue(const ABaseCharacter* owner) const {
	auto abilitySystem = owner->GetAbilitySystemComponent();
	Swap(abilitySystem->ScopedPredictionKey ,CurrentPrediction);

	FGameplayCueParameters parameters;
	parameters.Location = owner->GetActorLocation();
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.LightFeather.Landing"), parameters);

	Swap(abilitySystem->ScopedPredictionKey ,CurrentPrediction);
}

void ALightFeatherInstance::ApplyStunEffectToMobs(const ABaseCharacter* owner) const {
	const auto abilitySystem = owner->GetAbilitySystemComponent();
	const auto spec = CreateStunEffectSpec(abilitySystem);
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		
	for (auto mob : GetMobsToTarget(owner)) {
		if(HasAuthority()) pushback::pushback(MobPush, *owner, *mob);
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent(), CurrentPrediction);
	}
}

FGameplayEffectSpec ALightFeatherInstance::CreateStunEffectSpec(const UAbilitySystemComponent* abilitySystem) const {
	auto spec = FGameplayEffectSpec(Cast<const UGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	const auto duration = CalculateStunDuration();
	spec.SetSetByCallerMagnitude(effects::DurationName, duration);
	return spec;
}

TArray<AMobCharacter*> ALightFeatherInstance::GetMobsToTarget(const ABaseCharacter* player) const {
	const auto mobsInRange = actorquery::getNearbyActors<AMobCharacter>(player, 200.0f);
	return mobsInRange.FilterByPredicate([=](const AMobCharacter* mob) { return !player->IsFriendlyTowards(mob) && mob->IsTargetable(); });
}

float ALightFeatherInstance::CalculateStunDuration() const {
	const auto itemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	auto duration = StunDurationSeconds * itemPowerMultiplier;
	duration *= mDifficultyStats->GetMobStunDurationMultiplier();
	return duration;
}

void ALightFeatherInstance::Activate(const FPredictionKey& predictionKey) {
	CurrentPrediction = predictionKey;
	
	auto owner = Cast<APlayerCharacter>(GetOwner());
	
	owner->OnExternalDodgeRollStart(owner->GetActorForwardVector(), predictionKey);
	ExecuteLightFeatherJumpGameplayCue(owner);
	if (onLandedHandle.IsValid()) {
		owner->OnCharacterWalking.Remove(onLandedHandle);
		onLandedHandle.Reset();
	}
	onLandedHandle = owner->OnCharacterWalking.AddUObject(this, &ALightFeatherInstance::OnPlayerLanded);	


	Super::Activate(predictionKey);
}

void ALightFeatherInstance::ExecuteLightFeatherJumpGameplayCue(const ABaseCharacter* owner) const {
	const auto abilitySystem = owner->GetAbilitySystemComponent();

	FGameplayCueParameters parameters;
	parameters.Location = owner->GetActorLocation();
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.LightFeather.Jump"), parameters);
}
