#include "Dungeons.h"
#include "IronHideAmuletInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName DefenceEffectDuration(TEXT("DefenceDuration"));
	FName DefenceEffectMagnitude(TEXT("DefenceMagnitude"));
}

AIronHideAmuletInstance::AIronHideAmuletInstance() {
	Effect = UDefenceGameplayEffect::StaticClass();
	TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Defence"));
	PowerEffects = { UDurationIncrease::StaticClass() };
}

int AIronHideAmuletInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void AIronHideAmuletInstance::Activate(const FPredictionKey& predictionKey) {
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	abilitySystem->ExecuteGameplayCue(TriggerTag, FGameplayCueParameters());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<UDefenceGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	
	const float ItemPowerMultiplierDuration = GetPowerEffect()->GetMultiplier(ItemPower);
	const float duration = game::item::type::IronHideAmulet.getDurationSeconds() * ItemPowerMultiplierDuration;
	spec.SetSetByCallerMagnitude(effects::DurationName, duration);

	//const float ItemPowerMultiplier = game::item::power::HiddenDamageReductionMultiplier(ItemPower);
	spec.SetSetByCallerMagnitude(::DefenceEffectMagnitude, 1.f/DefenceBoostAmount);

	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	auto friendly = characterquery::is::friendly(characterOwner);
	for (auto friendChar : actorquery::getNearbyActors<ABaseCharacter>(characterOwner, ApplyToAlliesRange).FilterByPredicate(friendly)) {		
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, friendChar->GetAbilitySystemComponent(), predictionKey);
	}
	Super::Activate(predictionKey);
}

UDefenceGameplayEffect::UDefenceGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo defenceInfo;
	defenceInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat defenceMagnitude;
	defenceMagnitude.DataName = ::DefenceEffectMagnitude;

	defenceInfo.ModifierMagnitude = defenceMagnitude;
	defenceInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(defenceInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.DefenceUp")), 0, 1);
}