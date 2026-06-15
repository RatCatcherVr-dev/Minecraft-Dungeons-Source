#include "Dungeons.h"
#include "FrozenSolid.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"

// --- GAMEPLAY EFFECT --- //

UFrozenSolidGameplayEffect::UFrozenSolidGameplayEffect() {

	// Duration Setup
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;

	// Stacking Setup
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	DurationMagnitude = durationMagnitude;

	// Gameplay Effect Logic Setup
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;

	// Set speed to 0
	FGameplayModifierInfo speedModifier;
	speedModifier.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	speedModifier.ModifierOp = EGameplayModOp::Override;
	speedModifier.ModifierMagnitude = FScalableFloat(0);
	Modifiers.Add(speedModifier);

	// Set rotation to 0
	FGameplayModifierInfo rotationMultiplier;
	rotationMultiplier.Attribute = UMovementAttributeSet::RotationMultiplierAttribute();
	rotationMultiplier.ModifierOp = EGameplayModOp::Override;
	rotationMultiplier.ModifierMagnitude = FScalableFloat(0);
	Modifiers.Add(rotationMultiplier);

	// Place our effect into the gameplay cue array
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.FrozenSolid"), 0, 1);

	// Add gameplay tags
	const auto frozenSolidGameplayTag = FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.FrozenSolid");
	InheritableOwnedTagsContainer.AddTag(frozenSolidGameplayTag);
	InheritableGameplayEffectTags.AddTag(frozenSolidGameplayTag);
}