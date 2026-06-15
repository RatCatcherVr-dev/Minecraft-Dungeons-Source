#include "WindBowEnchantment.h"
#include "Dungeons.h"

#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/component/EnchantmentComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/util/Tags.h"
#include "game/util/Pushback.h"
#include "util/CharacterQuery.h"

UWindBowGameplayEffect::UWindBowGameplayEffect(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationMagnitude = durationMagnitude;

	// Get our unifying effect * resistance magnitude
	FCustomCalculationBasedFloat effectResistanceMagnitude;
	effectResistanceMagnitude.CalculationClassMagnitude = USlowMultiplicativeResistanceModCalculation::StaticClass();

	// Apply Move Speed Effect
	FGameplayModifierInfo moveSpeedInfo;
	moveSpeedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	moveSpeedInfo.ModifierMagnitude = effectResistanceMagnitude;
	moveSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(moveSpeedInfo);

	// Apply Tags
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Slow"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
}

UWindBowEnchantment::UWindBowEnchantment() {
	TypeId = EEnchantmentTypeID::WindBowEnchantment;
}

void UWindBowEnchantment::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!fromProjectile->IsCharged) {
		return;
	}

	if (fromProjectile->GetNumHits() >= 1) {
		if (auto projectileType = fromProjectile->GetProjectileItemType()) {
			if (projectileType.IsSet() && projectileType.GetValue() == game::item::type::TormentProjectile.getId()) {
				return;
			}
		}
	}
	 
	if (auto target = Cast<ABaseCharacter>(toWhom)) {
		auto attacker = Cast<ABaseCharacter>(GetOwner());
		auto attackerAbility = attacker->GetAbilitySystemComponent();
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(attackerAbility, Effect, 1.f);
		spec.SetSetByCallerMagnitude(effects::DurationName, StunTime);

		TArray<ABaseCharacter*> targets { target };

		auto hostile = characterquery::is::hostile(attacker);
		auto filter = [&](const ABaseCharacter* target) {
			return actorquery::is::alive(target) && hostile(target) && characterquery::is::targetable(target) && characterquery::is::movable(target) && !characterquery::is::boss(target);
		};

		auto targetmobs = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(target, KnockbackRadius).FilterByPredicate(filter);
		targets.Append(targetmobs);
		FVector launchVec = GetOwner()->GetActorLocation() - atLocation;
		launchVec.Normalize(0.01f);

		FPushback push;
		push.enablePushback = true;
		push.pushbackStrength = KnockbackStrength;
		push.pushbackZFactor = KnockbackZFactor;

		FGameplayCueParameters params;
		params.Location = target->GetActorLocation();
		params.Normal = launchVec;

		attackerAbility->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Windbow"), params);

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
		for (auto currentTarget : targets) {
			currentTarget->SetLastInjuredBy(attacker);
			if (currentTarget->HasAuthority()) {
				pushback::pushback(push, launchVec, *currentTarget);
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, launchVec, *currentTarget));
			}
			auto targetAbility = currentTarget->GetAbilitySystemComponent();
			attackerAbility->ApplyGameplayEffectSpecToTarget(spec, targetAbility);
		}

		if (GetOwner()->HasAuthority()) {
			ApplyRagdollImpulse(targets, push, launchVec);
		}
	}
}

void UWindBowEnchantment::ApplyRagdollImpulse_Implementation(const TArray<ABaseCharacter*>& targets, FPushback push, FVector launchVec) {
	for (auto currentTarget : targets) {
		if (currentTarget) {
			if (URagdollOnDeathComponent* ragdoll = currentTarget->FindComponentByClass<URagdollOnDeathComponent>()) {
				ragdoll->LaunchOrStoreRagdollImpulse(pushback::getLaunchVector(push, launchVec, *currentTarget));
			}
		}
	}
}