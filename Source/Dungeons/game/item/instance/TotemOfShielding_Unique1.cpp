// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TotemOfShielding_Unique1.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "Net/UnrealNetwork.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

UTotemOfShielding_Unique1DamageGameplayEffect::UTotemOfShielding_Unique1DamageGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Item")));
}

void ATotemOfShieldingActor_Unique1::OnDestroyCountdownStarted_Internal() {
	Super::OnDestroyCountdownStarted_Internal();
	
	if (auto owner = Cast<APlayerCharacter>(GetOwner())) {
		auto* abilitySystem = owner->GetAbilitySystemComponent();

		if (Role == ROLE_Authority) {
			if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 50, FColor::Green, false, 5.0f);

			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UTotemOfShielding_Unique1DamageGameplayEffect>(abilitySystem, Power);
			FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
			spec.SetSetByCallerMagnitude(effects::HealthName, -ExplosionDamage);
			context->AddInstigator(GetOwner(), this);
			context->AddOrigin(GetOwner()->GetActorLocation());
			auto nearbyMobs = actorquery::getNearbyActors<AMobCharacter>(this, ExplosionRadius);
			auto filteredMobs = nearbyMobs.FilterByPredicate([owner](AMobCharacter* mob) { return !owner->IsFriendlyTowards(mob) && mob->IsTargetable(); });

			for (auto actor : filteredMobs) {
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, actor->GetAbilitySystemComponent());
			}

			FGameplayCueParameters params;
			params.Instigator = owner;
			params.Location = GetActorLocation();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TotemOfShielding_Unique1.Explosion"), params);
		}
	}
}
