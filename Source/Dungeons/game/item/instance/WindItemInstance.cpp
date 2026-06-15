#include "Dungeons.h"
#include "WindItemInstance.h"

#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/util/Pushback.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

const FGameplayTag &levitationTag() {
	static const auto levitationStatusTag = FGameplayTag::RequestGameplayTag("StatusEffect.Levitation");
	return levitationStatusTag;
}

AWindItemInstance::AWindItemInstance() {
	Effect = UWindHornSlowGameplayEffect::StaticClass();
	PowerEffects = { UPushForceIncrease::StaticClass() };
}

float AWindItemInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::BlocksPushed:
		return WindPush.pushbackStrength;
	}
	return -1;
}

int AWindItemInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void AWindItemInstance::Activate(const FPredictionKey& predictionKey) {
	const auto ownerAsPlayer = Cast<APlayerCharacter>(GetOwner());

	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		const auto playerLocation = ownerAsPlayer->GetActorLocation();
		DrawDebugSphere(GetWorld(), playerLocation, PushRange, 50, FColor::Green, false, 5.0f);
	}

	ExecuteWindhornGameplayCue(ownerAsPlayer);
	ApplyWindhornEffectToMobs(ownerAsPlayer, predictionKey);

	Super::Activate(predictionKey);
}

void AWindItemInstance::ExecuteWindhornGameplayCue(const APlayerCharacter* player) const {
	auto abilitySystem = player->GetAbilitySystemComponent();

	FGameplayCueParameters parameters;
	parameters.Location = player->GetActorLocation();
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Windhorn"), parameters);
}

void AWindItemInstance::ApplyWindhornEffectToMobs(APlayerCharacter* player, const FPredictionKey& predictionKey) const {
	auto abilitySystem = player->GetAbilitySystemComponent();

	const auto pushbackStrength = WindPush.pushbackStrength * GetPowerEffect()->GetMultiplier(ItemPower);

	FPushback actualPush{ WindPush };
	actualPush.pushbackStrength = pushbackStrength;

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	const auto spec = CreateSlowEffectSpec(abilitySystem);
	
	for (auto mob : GetMobsToTarget(player)) {
		mob->SetLastInjuredBy(player);

		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent(), predictionKey);

		if (player->HasAuthority()) {
			// #D11.BC Windhorn pushback should not affect levitating mobs
			if (!mob->GetAbilitySystemComponent()->HasMatchingGameplayTag(levitationTag())) {
				pushback::pushback(actualPush, *player, *mob);
			}
		}
	}
}

TArray<AMobCharacter*> AWindItemInstance::GetMobsToTarget(const APlayerCharacter* player) const {
	const auto mobsInRange = actorquery::getNearbyActors<AMobCharacter>(player, PushRange);
	return mobsInRange.FilterByPredicate([=](const AMobCharacter* mob) { return !player->IsFriendlyTowards(mob); });
}

FGameplayEffectSpec AWindItemInstance::CreateSlowEffectSpec(UAbilitySystemComponent* abilitySystem) const {
	const float divider = 1.0f / SlowMultiplier;
	auto spec = FGameplayEffectSpec(Cast<UWindHornSlowGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1);
	spec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::DurationName, SlowDuration);
	spec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::SpeedDividerName, divider);
	spec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::MeleeSpeedDividerName, divider);
	spec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::RangedSpeedDividerName, divider);
	return spec;
}

const FName UWindHornSlowGameplayEffect::DurationName = FName("Duration");
const FName UWindHornSlowGameplayEffect::SpeedDividerName = FName("Speed");
const FName UWindHornSlowGameplayEffect::MeleeSpeedDividerName = FName("MeleeSpeed");
const FName UWindHornSlowGameplayEffect::RangedSpeedDividerName = FName("RangedSpeed");

UWindHornSlowGameplayEffect::UWindHornSlowGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = DurationName;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = SpeedDividerName;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedInfo.ModifierOp = EGameplayModOp::Type::Division;
	speedInfo.ModifierMagnitude = speedMagnitude;
	

	Modifiers.Add(speedInfo);

	FSetByCallerFloat meleeSpeedMagnitude;
	meleeSpeedMagnitude.DataName = MeleeSpeedDividerName;

	FGameplayModifierInfo meleeSpeedInfo;
	meleeSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	meleeSpeedInfo.ModifierMagnitude = meleeSpeedMagnitude;
	meleeSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(meleeSpeedInfo);

	FSetByCallerFloat rangedSpeedMagnitude;
	rangedSpeedMagnitude.DataName = RangedSpeedDividerName;

	FGameplayModifierInfo rangedSpeedInfo;
	rangedSpeedInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();

	rangedSpeedInfo.ModifierMagnitude = rangedSpeedMagnitude;
	rangedSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(rangedSpeedInfo);
	
	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Slow.Windhorn")), 0, 1);
}