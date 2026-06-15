#include "PillarVoidBlast.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "VoidTouched.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"

UPillarVoidBlastGameplayEffect::UPillarVoidBlastGameplayEffect(const FObjectInitializer& ObjectInitializer) {
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


UPillarVoidBlast::UPillarVoidBlast()
{
	TypeId = EEnchantmentTypeID::VoidBlast;

}

void UPillarVoidBlast::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter) {
		return;
	}

	if (UAbilitySystemComponent* ownerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>()) {

		FGameplayCueParameters params;
		params.Location = ownerCharacter->GetActorLocation();
		ownerAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.VoidBlast"), params);

		FPushback push;
		push.pushbackStrength = 5.0f;
		push.pushbackZFactor = 1.0f;
		push.enablePushback = true;

		// Create specs
		FGameplayEffectSpec damageSpec = effects::CreateGameplayEffectSpec<UPillarVoidBlastGameplayEffect>(ownerAbilitySystem, effects::HealthName, -blastDamage, GetOwner(), GetOwner(), GetOwner()->GetActorLocation(), 1);

		auto stunSpec = FGameplayEffectSpec(Cast<const UGameplayEffect>(StunEffect->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext());
		stunSpec.SetSetByCallerMagnitude(effects::DurationName, stunDuration);

		auto stunImmunitySpec = effects::CreateGameplayEffectSpec<UTemporaryStunimmunityGameplayEffect>(ownerAbilitySystem, 1.f);
		stunImmunitySpec.SetSetByCallerMagnitude(FName("Duration"), 2.f);

		FGameplayEffectSpec voidMagnitudeExtenderSpec(Cast<UVoidLiquidDurationExtenderGameplayEffect>(UVoidLiquidDurationExtenderGameplayEffect::StaticClass()->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext(), 1);
		voidMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), voidTouchDuration);
		FGameplayEffectSpec voidSpec(Cast<UVoidLiquidGameplayEffect>(UVoidLiquidGameplayEffect::StaticClass()->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext(), 1);
		voidSpec.SetSetByCallerMagnitude(FName("Duration"), voidTouchDuration);
		voidSpec.SetSetByCallerMagnitude(FName("Magnitude"), 0.0f);

		const auto targetableActors = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(GetWorld(), ownerCharacter->GetActorLocation(), blastRange).FilterByPredicate([&](ABaseCharacter* target) { return ownerCharacter->CanDamageTarget(target); });
		for (auto target : targetableActors) {
			if (UAbilitySystemComponent* targetAbilitySystem = target->FindComponentByClass<UAbilitySystemComponent>()) {
				if (target->IsAlive()) {

					// Apply a stun and a more potent stun immunity
					if (ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(stunSpec, targetAbilitySystem).WasSuccessfullyApplied()) {
						ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(stunImmunitySpec, targetAbilitySystem);
						ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(voidSpec, targetAbilitySystem);
						ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem);
						ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(voidMagnitudeExtenderSpec, targetAbilitySystem);

					}

					pushback::pushback(push, *ownerCharacter, *target);
					effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(push, *ownerCharacter, *target, 1.5f, 1.0f));
				}
			}
		}
	}
}