#include "Dungeons.h"
#include "IcePotionInstance.h"

#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"

#include <GameplayEffect.h>

AIcePotionInstance::AIcePotionInstance() {
	Effect = UIcePotionGameplayEffect::StaticClass();
}

int AIcePotionInstance::GetDisplayCount() const {
	return -1;
}

void AIcePotionInstance::Activate(const FPredictionKey& predictionKey) {
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UIcePotionGameplayEffect>(Effect->GetDefaultObject());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::IcePotion.getDurationSeconds());
	spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 1.0f/DefenseBoost);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}

//-------------------------------------------------------------------------------------------

UIcePotionGameplayEffect::UIcePotionGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.Ice"), 0, 1);

	// Damage resistance
	FGameplayModifierInfo iceInfo;
	iceInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	FSetByCallerFloat iceMagnitude;
	iceMagnitude.DataName = *UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName();
	iceInfo.ModifierMagnitude = iceMagnitude;
	iceInfo.ModifierOp = EGameplayModOp::Multiplicitive;
	Modifiers.Add( iceInfo );

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	// Freezing (Slow) immunity
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Slow"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Slow"));
}


