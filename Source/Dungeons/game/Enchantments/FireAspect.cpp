#include "Dungeons.h"
#include "FireAspect.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "util/CharacterQuery.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace FireAspectMagnitudes {
	FName FireAspectDuration(TEXT("FireAspectDuration"));
	FName FireAspectDamagePerSecondMagnitude(TEXT("FireAspectDamagePerSecondMagnitude"));
}

UFireAspectGameplayEffect::UFireAspectGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FireAspectMagnitudes::FireAspectDuration;
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.2f;

	FCustomCalculationBasedFloat damageMagnitude;
	damageMagnitude.CalculationClassMagnitude = UMeleeItemPowerOnlyModDamageCalculation::StaticClass();
	damageMagnitude.Coefficient = Period.GetValueAtLevel(1);

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = damageMagnitude;
	Modifiers.Add(info);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak.Fire"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));


	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.FireAspect"), 0, 1);

	const auto burningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Fire.Burning");
	InheritableOwnedTagsContainer.AddTag(burningEffectTag);
	InheritableGameplayEffectTags.AddTag(burningEffectTag);
}

UFireAspect::UFireAspect() {
	TypeId = EEnchantmentTypeID::FireAspect;
	Effect = UFireAspectGameplayEffect::StaticClass();
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return (1.0f + 0.25 * (level-1)) * DamagePerSecond;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage; 
}

void UFireAspect::DebuffTarget(ABaseCharacter* target, FSharedPredictionContext context) {
	auto characterOwner = GetCharacterOwner();

	if (!target || !characterquery::is::targetable(target) || !actorquery::is::alive(target)) return;

	if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UFireAspectGameplayEffect>(abilitySystem, Level);
	spec.SetSetByCallerMagnitude(effects::HealthName, IsOwnerMob() ? -MobDamagePerSecond : -LevelMultiplier(Level));
	spec.SetSetByCallerMagnitude(FireAspectMagnitudes::FireAspectDuration, fireDuration);
	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent(), context.GetKey());

}
FText UFireAspect::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(fireDuration)));
}

void UFireAspect::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	DebuffTarget(Cast<ABaseCharacter>(toWhat), context);
}

void UFireAspect::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	DebuffTarget(Cast<ABaseCharacter>(toWhat), FSharedPredictionContext());
}
