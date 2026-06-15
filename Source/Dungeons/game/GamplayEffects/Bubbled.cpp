#include "Bubbled.h"
#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "world/entity/MobTags.h"
#include "../abilities/effects/calculations/DamageModCalculations.h"
#include "../abilities/effects/executions/DamageExecutionCalculation.h"

namespace {
	FName SpeedFactorName = TEXT("SpeedFactor");
}

UBubbledGameplayEffect::UBubbledGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FScalableFloat ScalableFloatDuration;
	ScalableFloatDuration.SetValue(Duration);
	FGameplayEffectModifierMagnitude GEDurationMagnitude(ScalableFloatDuration);
	DurationMagnitude = GEDurationMagnitude;

	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;

	FGameplayModifierInfo speedFactorInfo;
	speedFactorInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = SpeedFactorName;
	speedFactorInfo.ModifierMagnitude = speedMagnitude;
	speedFactorInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(speedFactorInfo);

	FGameplayModifierInfo infoGravity;
	infoGravity.Attribute = UMovementAttributeSet::GravityAttribute();
	FScalableFloat ScalableFloatGravity;
	ScalableFloatGravity.SetValue(0.001f);
	FGameplayEffectModifierMagnitude ModifierGravityMagnitude(ScalableFloatGravity);
	infoGravity.ModifierMagnitude = ModifierGravityMagnitude;
	infoGravity.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(infoGravity);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Bubbled"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Bubbled"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Bubbled"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
}

void UBubbledHelper::ApplyBubbleEffect(TSubclassOf<UBubbledGameplayEffect> BubbleEffect, AActor* InstigatorActor, AActor* InstigatorProjectile, AActor* ActorToBubble) {

	if (InstigatorActor && ActorToBubble) {
		if (!InstigatorActor->HasAuthority()) {
			return;
		}

		if (hasMobTag(ActorToBubble, MobTags::HashTag_Unbubbled) ||
			hasMobTag(ActorToBubble, MobTags::HashTag_Summoner) //No necromancers
			|| hasMobTag(ActorToBubble, MobTags::HashTag_Caster) //No wraiths, enchanters, geomancers (until we can make them actually attack proper)...
			|| hasMobTag(ActorToBubble, MobTags::HashTag_Miniboss)
			|| hasMobTag(ActorToBubble, MobTags::HashTag_Unlovable)
			|| hasMobTag(ActorToBubble, MobTags::HashTag_EventMob))
		{
			return;
		}

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::RawMagnitude);
		if (UAbilitySystemComponent* BubbledAbilitySystemComponent = ActorToBubble->FindComponentByClass<UAbilitySystemComponent>()) {
			float magnitude = 0;
			if (InstigatorProjectile) {
				ABaseProjectile* projectile = Cast<ABaseProjectile>(InstigatorProjectile);
				magnitude = projectile->GetDamage();
			}
			FGameplayEffectSpec targetspec = effects::CreateGameplayEffectSpec<UBubbledGameplayEffect>(BubbledAbilitySystemComponent, 1.f);
			targetspec.SetSetByCallerMagnitude(SpeedFactorName, -magnitude);
			targetspec.GetContext().AddInstigator(InstigatorActor, InstigatorProjectile);
			if (BubbledAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(targetspec).WasSuccessfullyApplied()) {
				if (APlayerCharacter* character = Cast<APlayerCharacter>(InstigatorActor)) {
					character->OnBubbledEnemy(true);
				}
			}
		}
	}
}

void UBubbledHelper::DealDamageOnRadius(TSubclassOf<UBubbledDamageGameplayEffect> DamageEffect, float magnitude, AActor* BubbledActor, AActor* EffectInstigator) {
	if (!BubbledActor) {
		return;
	}

	if (!EffectInstigator || !EffectInstigator->HasAuthority()) {
		return;
	}

	UBubbledDamageGameplayEffect* Effect = Cast<UBubbledDamageGameplayEffect>(DamageEffect->GetDefaultObject());
	for (auto actor : actorquery::getNearbyActors<ABaseCharacter>(BubbledActor->GetWorld(), BubbledActor->GetActorLocation(), Effect->PopDamageRadius)) {
		if (const AMobCharacter* mob = Cast<AMobCharacter>(actor)) {
			if (mob->IsAlive())  {
				auto AbilitySystemComponent = mob->GetAbilitySystemComponent();

				const FVector distToEffectCenter = mob->GetActorLocation() - BubbledActor->GetActorLocation();
				float launchMagnitude = distToEffectCenter.Size() / 900.0f;
				launchMagnitude = 1.0f - launchMagnitude;

				FPushback push;
				push.pushbackStrength = launchMagnitude * 2.0f;
				push.pushbackStrength = FMath::Max(push.pushbackStrength, 2.0f);
				push.pushbackZFactor = 2.0f;
				push.enablePushback = true;

				pushback::pushback(push, *BubbledActor, *mob);

				if (UAbilitySystemComponent* InstigatorAbilitySystemComponent = EffectInstigator->FindComponentByClass<UAbilitySystemComponent>()) {
					FGameplayEffectSpec DamageSpec = effects::CreateGameplayEffectSpec<UBubbledDamageGameplayEffect>(InstigatorAbilitySystemComponent, effects::HealthName, magnitude, EffectInstigator, EffectInstigator, BubbledActor->GetActorLocation(), 1.f);
					effects::StorePushbackInNormal(DamageSpec, pushback::getLaunchVector(push, *BubbledActor, *mob, 1.5f, 1.0f));
					InstigatorAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(DamageSpec, AbilitySystemComponent);
				}
			}
		}
	}

	if (EffectInstigator) {
		if (APlayerCharacter* character = Cast<APlayerCharacter>(EffectInstigator)) {
			character->OnBubbledEnemy(false);
		}
	}
}

UBubbledDamageGameplayEffect::UBubbledDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UReflectedRangeItemPowerDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}
