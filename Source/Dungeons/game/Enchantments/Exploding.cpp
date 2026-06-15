#include "Dungeons.h"
#include "Exploding.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "util/CharacterQuery.h"
#include "../Game.h"

UExplodingDamageGameplayEffect::UExplodingDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	InheritableGameplayEffectTags.AddTag(damageTag::explosion());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Explosion"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UExploding::UExploding() {
	TypeId = EEnchantmentTypeID::Exploding;
	LevelMultiplier = [this](int level) -> float {
		return explosionMaxHealthFactorBase + explosionMaxHealthFactorPerLevel*(level-1);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UExploding::OnExplodeCharacter(TWeakObjectPtr<ABaseCharacter> target) {
	if (!target.IsValid()) return;

	auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());

	FVector meshLocation = target->GetMesh()->GetComponentToWorld().GetLocation();

	auto canDamage = characterquery::can::damage(ownerCharacter);
	auto filter = [&](const ABaseCharacter* target) {
		return actorquery::is::alive(target) && canDamage(target) && characterquery::is::targetable(target);
	};

	auto targets = actorquery::getNearbyActors<ABaseCharacter>(GetWorld(), meshLocation, explosionRadius).FilterByPredicate(filter);

	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	FGameplayCueParameters params;
	params.Location = meshLocation;
	params.NormalizedMagnitude = Level / 3.f;
	params.Instigator = ownerCharacter;

	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Explode"), params);

	FPushback push;
	push.pushbackStrength = 5.0f;
	push.pushbackZFactor = 1.0f;
	push.enablePushback = true;

	if (const auto game = actorquery::getGame(GetWorld())) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
		for (ABaseCharacter* explosionTarget : targets) {
			pushback::pushback(push, *target, *explosionTarget);

			auto targetAbilitySystem = explosionTarget->GetAbilitySystemComponent();
			if (auto targetHc = target->FindComponentByClass<UHealthComponent>()) {
				const float damage = targetHc->GetMaximumHealth() * LevelMultiplier(Level);
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UExplodingDamageGameplayEffect>(abilitySystem, effects::HealthName, -damage, GetOwner(), GetOwner(), GetOwner()->GetActorLocation(), Level);
				//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, *target, *explosionTarget, 1.5f, 1.0f));
				effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
			}
		}
	}

	target->GetMesh()->SetVisibility(false);
}

void UExploding::SpawnExplosion(ABaseCharacter* target) {
	if (GetOwner()->Role != ROLE_Authority) {
		return;
	}
	if (!actorquery::is::alive(target)) {
		TWeakObjectPtr<ABaseCharacter> explosionTarget = target;

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UExploding::OnExplodeCharacter, explosionTarget), 1.0f, false);
	}
}

void UExploding::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	SpawnExplosion(Cast<ABaseCharacter>(toWhom));
}

void UExploding::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	SpawnExplosion(Cast<ABaseCharacter>(toWhom));
}
