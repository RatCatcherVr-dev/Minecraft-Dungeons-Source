#include "Dungeons.h"
#include "ShockPowderInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

AShockPowderInstance::AShockPowderInstance() {
	PowerEffects = { UStunDurationIncrease::StaticClass() };
}

float AShockPowderInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::StunDuration:
		return game::item::type::ShockPowder.getDurationSeconds();
	}
	return -1;
}

int AShockPowderInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void AShockPowderInstance::Activate(const FPredictionKey& predictionKey) {
	const auto owner = GetOwner();

	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		DrawDebugSphere(GetWorld(), owner->GetActorLocation(), StunRange, 50, FColor::Green, false, 5.0f);	
	}

	const auto ownerAsPlayer = Cast<APlayerCharacter>(GetOwner());
	ApplyStunEffectToMobs(ownerAsPlayer, predictionKey);
	ExecuteStunGameplayCue(ownerAsPlayer);

	Super::Activate(predictionKey);
}

void AShockPowderInstance::ApplyStunEffectToMobs(const APlayerCharacter* player, const FPredictionKey& predictionKey) const {
	const auto abilitySystem = player->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	const auto spec = CreateStunEffectSpec(abilitySystem);
	
	for (const auto mob : GetMobsToTarget(player)) {
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent(), predictionKey);
	}
}

FGameplayEffectSpec AShockPowderInstance::CreateStunEffectSpec(const UAbilitySystemComponent* abilitySystem) const {
	const auto duration = CalculateDuration();
	auto spec = FGameplayEffectSpec(Cast<const UGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, duration);
	return spec;
}

float AShockPowderInstance::CalculateDuration() const {
	const auto itemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	auto duration = game::item::type::ShockPowder.getDurationSeconds() * itemPowerMultiplier;
	duration *= mDifficultyStats->GetMobStunDurationMultiplier();
	return duration;
}

TArray<AMobCharacter*> AShockPowderInstance::GetMobsToTarget(const APlayerCharacter* player) const {
	const auto mobsInRange = actorquery::getNearbyActors<AMobCharacter>(player, StunRange);
	return mobsInRange.FilterByPredicate([=](const AMobCharacter* mob) { return IsValidStunTarget(player, mob); });
}

bool AShockPowderInstance::IsValidStunTarget(const APlayerCharacter* player, const AMobCharacter* mob) {
	return mob->IsTargetable() && mob->IsAlive() && !player->IsFriendlyTowards(mob);
}

void AShockPowderInstance::ExecuteStunGameplayCue(const APlayerCharacter* player) {
	const auto abilitySystem = player->GetAbilitySystemComponent();

	FGameplayCueParameters params;
	params.Location = player->GetActorLocation();
	params.NormalizedMagnitude = 1.f;
	params.Instigator = player;
	params.EffectCauser = this;
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Stun"), params);
}
