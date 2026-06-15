#include "VoidBlast.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "VoidTouched.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"

UVoidBlastGameplayEffect::UVoidBlastGameplayEffect(const FObjectInitializer& ObjectInitializer) {
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

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

}


UVoidBlast::UVoidBlast()
{
	TypeId = EEnchantmentTypeID::VoidBlast;

}

void UVoidBlast::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter) {
		return;
	}

	if (UAbilitySystemComponent* ownerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>()) {

		FGameplayCueParameters params;
		params.Location = ownerCharacter->GetActorLocation();
		ownerAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.VoidBlast"), params);

		FPushback push;
		push.pushbackStrength = 3.0f;
		push.pushbackZFactor = 1.0f;
		push.enablePushback = true;

		// Create specs
		FGameplayEffectSpec damageSpec = effects::CreateGameplayEffectSpec<UVoidBlastGameplayEffect>(ownerAbilitySystem, effects::HealthName, -blastDamage, GetOwner(), GetOwner(), GetOwner()->GetActorLocation(), 1);

		const float divider = 1.0f / slowMultiplier;
		FGameplayEffectSpec slowSpec = FGameplayEffectSpec(Cast<UWindHornSlowGameplayEffect>(slowEffect->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext(), 1);
		slowSpec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::DurationName, slowDuration);
		slowSpec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::SpeedDividerName, divider);
		slowSpec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::MeleeSpeedDividerName, divider);
		slowSpec.SetSetByCallerMagnitude(UWindHornSlowGameplayEffect::RangedSpeedDividerName, divider);

		const auto targetableActors = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(GetWorld(), ownerCharacter->GetActorLocation(), blastRange).FilterByPredicate([&](ABaseCharacter* target) { return ownerCharacter->CanDamageTarget(target); });
		for (auto target : targetableActors) {
			if (UAbilitySystemComponent* targetAbilitySystem = target->FindComponentByClass<UAbilitySystemComponent>()) {
				if (target->IsAlive()) {
					pushback::pushback(push, *ownerCharacter, *target);
					effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(push, *ownerCharacter, *target, 1.5f, 1.0f));

					ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(slowSpec, targetAbilitySystem);
					ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem);
				}
			}
		}
	}
}