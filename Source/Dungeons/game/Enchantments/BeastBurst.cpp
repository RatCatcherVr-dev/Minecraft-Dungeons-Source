#include "Dungeons.h"
#include "BeastBurst.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/item/power/ItemPowerStats.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"


UBeastBurstExplosionGameplayEffect::UBeastBurstExplosionGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	
	InheritableGameplayEffectTags.AddTag(damageTag::explosion());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.BeastBurst"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}


UBeastBurst::UBeastBurst() {
	TypeId = EEnchantmentTypeID::BeastBurst;
	Effect = UBeastBurstExplosionGameplayEffect::StaticClass();
	LevelMultiplier = [this](int level) -> float {
		return BaseDamageValue + ((level - 1) * ExtraDamagePerLevel);
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void UBeastBurst::OnHealthPotionUsed() {
	auto owner = GetCharacterOwner();
	for (auto minion : owner->GetMinions()) {
		StartExplosion(minion);
	}

	BroadcastEnchantmentTriggeredEvent();
}

void UBeastBurst::StartExplosion(ABaseCharacter* minion) {

	auto owner = GetCharacterOwner();

	const auto hostilePredicate = characterquery::is::hostile(owner);
	const auto targetPredicate = [&](const ABaseCharacter* v) { return hostilePredicate(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };
	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(minion, ExplosionRadius).FilterByPredicate(targetPredicate);

	auto abilitySystem = GetAbilitySystemComponent();

	FGameplayCueParameters params;
	params.Location = minion->GetMesh()->GetComponentToWorld().GetLocation();
	params.NormalizedMagnitude = Level / 3.f;
	params.Instigator = owner;

	minion->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.BeastBurst.Explode"), params);

	FPushback push;
	push.pushbackStrength = 5.0f;
	push.pushbackZFactor = 1.0f;
	push.enablePushback = true;

	for (ABaseCharacter* explosionTarget : targets) {
		pushback::pushback(push, *minion, *explosionTarget);

		if (explosionTarget->GetHealthComponent()) {
			const float damage = LevelMultiplier(Level);
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UBeastBurstExplosionGameplayEffect>(
				abilitySystem, effects::HealthName, -damage,
				GetOwner(), GetOwner(), GetOwner()->GetActorLocation(), Level);

			//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
			effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, *minion, *explosionTarget, 1.5f, 1.0f));
			effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, explosionTarget->GetAbilitySystemComponent());
		}
	}
}
