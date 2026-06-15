// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Echo.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "TimerManager.h"

UEcho::UEcho() {
	TypeId = EEnchantmentTypeID::Echo;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseTriggerEchoTimeSeconds - (level - 1);
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
}

void UEcho::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	const auto characterOwner = GetCharacterOwner();
	if (bCanTrigger && !bIsInEcho) {
		if (const auto meleeAttackComponent = characterOwner->FindComponentByClass<UMeleeAttackComponent>()) {
			if (GetOwner()->HasAuthority()) {
				BroadcastEnchantmentTriggeredEvent();
			}

			bIsInEcho = true;
			
			if (!bAlwaysTrigger) {
				bCanTrigger = false;
				GetWorld()->GetTimerManager().SetTimer(ResetEchoTimerHandle, this, &UEcho::OnCanTriggerEcho, IsOwnerMob() ? MobTriggerEchoTimeSeconds : LevelMultiplier(Level));
			}
			
			FTimerHandle handle;
			const auto secondsRemaining = FMath::Clamp(meleeAttackComponent->CanAttackFromTimestampSeconds() - GetWorld()->GetTimeSeconds(), 0.f, MaxDelay);

			auto delegate = FTimerDelegate::CreateUObject(this, &UEcho::OnEcho, TWeakObjectPtr<AActor>(attackTarget), attackVector, index, static_cast<int32>(randStream.GetUnsignedInt()), window);
			GetWorld()->GetTimerManager().SetTimer(handle, delegate, secondsRemaining / 2.0f, false);
		}
	}
	else if (bIsInEcho) {
		bIsInEcho = false;
	}
}

void UEcho::OnEcho(TWeakObjectPtr<AActor> target, FVector attackVector, int32 index, int32 seed, FSharedPredictionContext context) {
	const auto characterOwner = GetCharacterOwner();
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	if (const auto meleeAttackComponent = characterOwner->FindComponentByClass<UMeleeAttackComponent>()) {
		FUseSpecfiedKeyScopedPredictionWindow window(context);

		FGameplayCueParameters params;
		params.Location = characterOwner->GetActorLocation();
		params.NormalizedMagnitude = Level / 3.0f;

		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Echo"), params);

		meleeAttackComponent->AttackWithVariantIndex(target.Get(), attackVector, index, seed, context);
	}
}

void UEcho::OnCanTriggerEcho() {
	bCanTrigger = true;
}
