#include "SharedPain.h"
#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/ai/provider/ActorStats.h"
#include "game/abilities/effects/DamageSelfGameplayEffect.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"

USharedPainDamageGameplayEffect::USharedPainDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Medium"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.SharedPain"), 0, 1);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}

USharedPain::USharedPain() {
	TypeId = EEnchantmentTypeID::SharedPain;
	PredictiveExecution = true;
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
}

void USharedPain::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window)
{
	if (!missedAttack && DamageOverflow < 0) {
		if (auto mob = Cast<AMobCharacter>(toWhat)) {
			const auto IsEnemy = [&](AMobCharacter* mob) {
				return mob->IsHostileTowardsPlayers() && mob->IsTargetable() && mob->IsAlive();
			};

			if (auto ownerPlayer = Cast<APlayerCharacter>(GetOwner())) {
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USharedPainDamageGameplayEffect>(ownerPlayer->GetAbilitySystemComponent(), 1);
				FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
				auto nearMobs = actorquery::getNearbyActors<AMobCharacter>(toWhat, MobsRange, IsEnemy);
				spec.SetSetByCallerMagnitude(effects::HealthName, DamageOverflow / nearMobs.Num());
				for (auto actor : nearMobs) {
					mob->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(spec, actor->GetAbilitySystemComponent());
				}
				if (GetOwnerRole() == ROLE_Authority) {
					BroadcastEnchantmentTriggeredEvent();
				}
				DamageOverflow = 0;
			}
		}
	}
}

void USharedPain::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) {
	if (bCanTriggerSharedPain)
	{
		if (auto mob = Cast<AMobCharacter>(targetComponent->GetAvatarActor())) {
			float currentHealth = 0;
			if (const auto healthComponent = mob->FindComponentByClass<UHealthComponent>()) {
				currentHealth = healthComponent->GetCurrentHealth();
			}

			float damage = mutableSpec.GetSetByCallerMagnitude(effects::HealthName);
			damage *= ItemPowerMultiplier(GetSourceItemPower());

			DamageOverflow = damage + currentHealth;
			DamageOverflow = Math::min(DamageOverflow, 0);
			bCanTriggerSharedPain = false;
		}
	}
}

void USharedPain::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	if (index >= 1)//will only trigger on the 2rd attack variant
	{
		bCanTriggerSharedPain = true;
	}
}
