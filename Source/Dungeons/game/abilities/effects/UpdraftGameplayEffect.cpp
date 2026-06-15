#include "Dungeons.h"
#include "UpdraftGameplayEffect.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/DamageModCalculations.h"
#include "executions/DamageExecutionCalculation.h"
#include "game/component/HealthComponent.h"

UBlastGameplayEffect::UBlastGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	InheritableGameplayEffectTags.AddTag(damageTag::wind());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Knockback.Windcaller")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.WindcallerBlast.Impact"), 0, 1);
}

UUpdraftGameplayEffect::UUpdraftGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	InheritableGameplayEffectTags.AddTag(damageTag::wind());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Knockback.Windcaller")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Updraft.Impact"), 0, 1);	
}

UUpdraftImmunityGameplayEffect::UUpdraftImmunityGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");
	DurationMagnitude = durationMagnitude;
	
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Immunity.Windcaller")));
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Knockback.Windcaller")));
}
