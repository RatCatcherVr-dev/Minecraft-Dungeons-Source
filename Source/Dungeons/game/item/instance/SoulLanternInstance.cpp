// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "SoulLanternInstance.h"
#include "game/abilities/attributes/MovementAttributeSet.h"

const FName USoulLanterenTeleportGameplayEffect::DurationName(TEXT("Duration"));

USoulLanterenTeleportGameplayEffect::USoulLanterenTeleportGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat duration;
	duration.DataName = DurationName;

	DurationMagnitude = duration;

	FGameplayModifierInfo speed;
	speed.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speed.Magnitude = FScalableFloat(0.f);
	speed.ModifierOp = EGameplayModOp::Override;

	Modifiers.Add(speed);
}

ASoulLanternInstance::ASoulLanternInstance() :Super() {
	mobType = "SoulWizard";
	TeleportEffect = USoulLanterenTeleportGameplayEffect::StaticClass();
}

TOptional<FGameplayEffectSpec> ASoulLanternInstance::GenerateTeleportEffectSpec() const {
	if (auto spec = Super::GenerateTeleportEffectSpec()) {
		spec->SetSetByCallerMagnitude(USoulLanterenTeleportGameplayEffect::DurationName, TeleportRootTime);
		return spec;
	}
	return {};
}