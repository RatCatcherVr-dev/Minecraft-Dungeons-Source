// Fill out your copyright notice in the Description page of Project Settings.

#include "ChainLightning.h"
#include "Dungeons.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/util/ActorQuery.h"
#include "util/CharacterQuery.h"
#include "game/mobspawn/MobAction.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/team/TeamQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;


UChainLightningDamageGameplayEffect::UChainLightningDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Damage.Aoe"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Medium"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.ChainLightning"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UMeleeChainLightningDamageGameplayEffect::UMeleeChainLightningDamageGameplayEffect() {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}


URangedChainLightningDamageGameplayEffect::URangedChainLightningDamageGameplayEffect() {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UItemPowerOnlyAsEffectLevelModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}

namespace chainlightning {

	void ApplyChainLightning(TWeakObjectPtr<AActor> fromTarget, TArray<TWeakObjectPtr<ABaseCharacter>> targets, float ChainLightningDelay, float IndividualChainRadius, FGameplayEffectSpec spec) {
		if (const auto target = fromTarget.Get()) {
			ETeamName instigatorTeam = effects::GetDungeonsContextFromSpec(spec)->InstigatorTeam;

			const auto predicate = [&](const TWeakObjectPtr<ABaseCharacter> v) { return !v.IsValid() || !actorquery::is::alive(v.Get()) || !characterquery::is::targetable(v.Get()) || !teamquery::is::hostile(v->GetCurrentTeam(), instigatorTeam); };
			targets.RemoveAllSwap(predicate);

			targets.Sort([&](const TWeakObjectPtr<ABaseCharacter> a, const TWeakObjectPtr<ABaseCharacter> b) {
				return actorquery::getActorDistance(target, a.Get()) > actorquery::getActorDistance(target, b.Get());
			});
			//Num must be checked because pop crashes on length 0
			while (targets.Num()) {
				auto newTarget = targets.Pop();

				if (newTarget.IsValid()) {
					FHitResult res;
					const auto hitTerrain = newTarget->GetWorld()->LineTraceSingleByChannel(res, target->GetActorLocation(), newTarget->GetActorLocation(), (ECollisionChannel)ECustomTraceChannels::TerrainOnly);

					if (!hitTerrain && actorquery::getActorDistance(target, newTarget.Get()) <= IndividualChainRadius) {
						spec.GetContext().AddInstigator(spec.GetContext().GetInstigator(), target->IsA<ABaseCharacter>() ? target : nullptr);
						spec.GetContext().AddOrigin(target->GetActorLocation());
						FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location | EGameplayCueParametersField::EffectCauser);
						newTarget->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(spec);

						if (targets.Num()) {
							FTimerHandle handle;
							TWeakObjectPtr<AActor> weakFrom = newTarget;
							target->GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateStatic(ApplyChainLightning, weakFrom, targets, ChainLightningDelay, IndividualChainRadius, spec), ChainLightningDelay, false);
							break;
						}
					}
				}
			}
		}
	}
}