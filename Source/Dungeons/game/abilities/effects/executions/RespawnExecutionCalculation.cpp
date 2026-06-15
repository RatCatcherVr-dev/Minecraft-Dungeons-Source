#include "Dungeons.h"
#include "RespawnExecutionCalculation.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/DamageHelpers.h"

void URespawnExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	AActor* actorToKill = nullptr;

	if (auto targetSystem = ExecutionParams.GetTargetAbilitySystemComponent()) {
		actorToKill = targetSystem->GetOwner();
	}

	if (actorToKill) {
		const bool isFreeze = ExecutionParams.GetPassedInTags().HasTag(FGameplayTag::RequestGameplayTag(TEXT("Respawn.Freeze")));
		damagehelpers::tryKillByKillZone(*actorToKill, isFreeze);
	}
}
