#include "Dungeons.h" 
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "VoidBlockTrigger.h"
#include "game/Conversion.h"
#include "util/CharacterQuery.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "world/entity/MobTags.h" 
#include "game/actor/character/DungeonsAbilitySystemComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "Internationalization.h"
#include "SoftObjectPtr.h"
#include "game/GameTypes.h"

UVoidBlockTrigger::UVoidBlockTrigger() : UBlockTrigger()
{
	slowdownGameplayEffect = Cast<UVoidBlockSlowdownGameplayEffect>(UVoidBlockSlowdownGameplayEffect::StaticClass()->GetDefaultObject());
	damageGameplayEffect = Cast<UVoidBlockDamageGameplayEffect>(UVoidBlockDamageGameplayEffect::StaticClass()->GetDefaultObject());
	durationExtenderGameplayEffect = Cast<UVoidBlockDurationExtenderGameplayEffect>(UVoidBlockDurationExtenderGameplayEffect::StaticClass()->GetDefaultObject());
	magntiudeExtenderGameplayEffect = Cast<UVoidBlockMagnitudeExtenderGameplayEffect>(UVoidBlockMagnitudeExtenderGameplayEffect::StaticClass()->GetDefaultObject());
}

void UVoidBlockTrigger::OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	if (characterquery::is::targetable(character) && actorquery::is::alive(character))
	{
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

		FGameplayEffectQuery query;
		query.EffectDefinition = UVoidBlockGameplayEffect::StaticClass();
		auto effects = character->GetAbilitySystemComponent()->GetActiveEffects(query);
		if (effects.Num() == 0)
		{
			FGameplayEffectSpec effectSpec(Cast<UVoidBlockGameplayEffect>(UVoidBlockGameplayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
			effectSpec.SetSetByCallerMagnitude(FName("Duration"), baseVoidBlockDuration);
			effectSpec.SetSetByCallerMagnitude(FName("Magnitude"), baseDamageMultiplier);
			character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectSpec);
		}

		FGameplayEffectSpec effectDurationExtenderSpec(durationExtenderGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), -1);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseVoidBlockDuration);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseDamageMultiplier);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectDurationExtenderSpec);

		FGameplayEffectSpec effectMagnitudeExtenderSpec(magntiudeExtenderGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), -1);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseVoidBlockDuration);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseDamageMultiplier);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectMagnitudeExtenderSpec);

		FGameplayEffectSpec damageSpec(damageGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(damageSpec);

		ApplySlowdown(character, true);
	}
}

void UVoidBlockTrigger::ApplySlowdown(ABaseCharacter* const character, bool isEnter) const
{
	character->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.VoidBlock.Slow")));

	FGameplayEffectSpec speedSpec(slowdownGameplayEffect, character->AbilitySystem->MakeEffectContext(), 1);
	if (isEnter)
	{
		speedSpec.StackCount = initialSlowdownPercentage - slowDownPercentage;
		speedSpec.SetSetByCallerMagnitude(TEXT("Duration"), initialSlowdownFalloffSpeed);
		character->AbilitySystem->ApplyGameplayEffectSpecToSelf(speedSpec);

		speedSpec.StackCount = slowDownPercentage;
		speedSpec.SetSetByCallerMagnitude(TEXT("Duration"), -1);
		character->AbilitySystem->ApplyGameplayEffectSpecToSelf(speedSpec);
	}
	else
	{
		speedSpec.StackCount = slowDownPercentage;
		speedSpec.SetSetByCallerMagnitude(TEXT("Duration"), exitSlowDownFalloffSpeed);
		character->AbilitySystem->ApplyGameplayEffectSpecToSelf(speedSpec);
	}
}

void UVoidBlockTrigger::OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	if (characterquery::is::targetable(character) && actorquery::is::alive(character))
	{
		if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(character))
		{
			FGameplayEffectQuery query;
			query.EffectDefinition = UVoidBlockGameplayEffect::StaticClass();
			TArray<FActiveGameplayEffectHandle> effects = character->AbilitySystem->GetActiveEffects(query);
			if (effects.Num() > 0)
			{
				float magnitude = character->AbilitySystem->GetGameplayEffectMagnitude(effects[0], UHealthAttributeSet::TakeDamageMultiplierAttribute()) + 1;
				playerCharacter->VoidEffectMagnitude = magnitude;
				playerCharacter->OnRep_VoidEffectMagnitude();
			}
		}
		character->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.VoidBlock")));
		character->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Damage.VoidBlock")));
		ApplySlowdown(character, false);
	}
}

bool UVoidBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	const AMobCharacter* mob = Cast<AMobCharacter>(character);
	const bool isVoidBlock = fullBlock.getBlock().getId() == Block::mVoidBlock->getId();
	const bool isEnderMob = mob && hasMobTag(mob->EntityType, MobTags::HashTag_Ender);
	const bool isCosmeticMob = mob && hasMobTag(mob->EntityType, MobTags::HashTag_Cosmetic);
	return isVoidBlock && !isEnderMob && !isCosmeticMob;
}

UVoidBlockGameplayEffect::UVoidBlockGameplayEffect() : UVoidLiquidGameplayEffect()
{
	GameplayCues.Reset();
	InheritableGameplayEffectTags.CombinedTags.Reset();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Voided")), 0, 1);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Voided"));
	UIData = UVoidedGameplayEffectUIData::StaticClass()->GetDefaultObject<UVoidedGameplayEffectUIData>();
}

UVoidBlockDurationExtenderGameplayEffect::UVoidBlockDurationExtenderGameplayEffect() : UVoidLiquidDurationExtenderGameplayEffect()
{
	durationIncreasePeriod = 0.5f;

	Period = durationIncreasePeriod;

	Executions.Reset();

	FGameplayEffectExecutionDefinition voidBlockDurationExtenderExecution;
	voidBlockDurationExtenderExecution.CalculationClass = UVoidBlockDurationExtenderExecution::StaticClass();
	Executions.Add(std::move(voidBlockDurationExtenderExecution));

	InheritableGameplayEffectTags.CombinedTags.Reset();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidBlock"));
}

UVoidBlockDurationExtenderExecution::UVoidBlockDurationExtenderExecution()
{
	effectToApply = UVoidBlockGameplayEffect::StaticClass();
	maxDuration = 99.0f;
	exponetialIncrease = 1.05f;
	periodIncrease = 0.4f;
}

UVoidBlockMagnitudeExtenderGameplayEffect::UVoidBlockMagnitudeExtenderGameplayEffect() : UVoidLiquidMagnitudeExtenderGameplayEffect()
{
	magnitudeIncreasePeriod = 1.0f;

	Period = magnitudeIncreasePeriod;

	Executions.Reset();

	FGameplayEffectExecutionDefinition voidBlockMagnitudeExtenderExecution;
	voidBlockMagnitudeExtenderExecution.CalculationClass = UVoidBlockMagnitudeExtenderExecution::StaticClass();
	Executions.Add(std::move(voidBlockMagnitudeExtenderExecution));

	InheritableGameplayEffectTags.CombinedTags.Reset();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidBlock"));
}

UVoidBlockMagnitudeExtenderExecution::UVoidBlockMagnitudeExtenderExecution()
{
	effectToApply = UVoidBlockGameplayEffect::StaticClass();
	exponetialIncrease = 1.12f;
	periodIncrease = 0.5f;
	maxMagnitude = 999.f;
}

UVoidBlockSlowdownGameplayEffect::UVoidBlockSlowdownGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");

	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	info.ModifierMagnitude = FScalableFloat(-0.01f);
	info.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidBlock.Slow"));
}

UVoidBlockDamageGameplayEffect::UVoidBlockDamageGameplayEffect() : UVoidLiquidDamageGameplayEffect()
{
	damagePerPeriod = 5.0f;
	damagePeriod = 1.0f;

	Modifiers.Reset();

	Period = damagePeriod;

	FAttributeBasedFloat damageCalc;
	damageCalc.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	damageCalc.Coefficient = -damagePerPeriod * 0.01f;
	damageCalc.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	damageCalc.BackingAttribute.bSnapshot = true;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = damageCalc;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.CombinedTags.Reset();

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.VoidBlock"));
	InheritableGameplayEffectTags.AddTag(damageTag::environmental());
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}

UVoidedGameplayEffectUIData::UVoidedGameplayEffectUIData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	name = NSLOCTEXT("StatusEffect", "voided", "Voided");
	softIconClassRef = TSoftClassPtr<UUserWidget>(game::PrefabPath("UI/StatusEffects/GameplayEffect/Icons/UMG_VoidedIcon.UMG_VoidedIcon_C"));
	displayPreference = EStackCountDisplayPreference::AsMagnitude;
	displayMagnitudeAttribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	displayMagnitudeOffset = 1.0f;
	displayMagnitudeFractionalDigitsCount = 0;
	displayDurationUpdateFrequency = 0.5f;
}