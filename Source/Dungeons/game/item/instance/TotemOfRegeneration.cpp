#include "Dungeons.h"
#include "TotemOfRegeneration.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "TotemOfSpiritProtection.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"

UTotemOfRegenerationGameplayEffect::UTotemOfRegenerationGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.2f;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = Period.GetValueAtLevel(1);

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = healingMagnitude;
	Modifiers.Add(info);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Regeneration"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

ATotemOfRegenerationActor::ATotemOfRegenerationActor() {
	Effect = UTotemOfRegenerationGameplayEffect::StaticClass();
}



void ATotemOfRegenerationActor::PreBuffComponentBeginPlay(UAreaBuffComponent* BuffComponent) {
	BuffComponent->Attributes.Emplace(UHealthAttributeSet::HealthAttribute(), TotalHealthHealedPerSecond);
}

ATotemOfRegenerationInstance::ATotemOfRegenerationInstance()
{
	PowerEffects = { UHealingIncrease::StaticClass() };
}

float ATotemOfRegenerationInstance::GetStats(EItemStats stat) const {
	const auto& itemType = GetItemType();
	switch (stat) {
	case EItemStats::LowestHealthHealed:		
	case EItemStats::HighestHealthHealed:
		return itemType.getDurationSeconds() * HealthHealedPerSecond;
	}
	return -1;
}


void ATotemOfRegenerationInstance::PreTotemBeginPlay(ATotemBaseActor* totemActor) {
	const auto& itemType = GetItemType();
	if (auto* regenerationTotem = Cast<ATotemOfRegenerationActor>(totemActor)) {
		regenerationTotem->Duration = itemType.getDurationSeconds();
		regenerationTotem->TotalHealthHealedPerSecond = GetPowerEffect(0)->GetMultiplier(ItemPower) * HealthHealedPerSecond;
	}
}	
