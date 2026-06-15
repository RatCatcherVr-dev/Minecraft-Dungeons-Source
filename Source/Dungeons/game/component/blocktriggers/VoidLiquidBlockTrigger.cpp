#include "Dungeons.h" 
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "VoidLiquidBlockTrigger.h"
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

UVoidLiquidBlockTrigger::UVoidLiquidBlockTrigger() : ULavaBlockTrigger()
{
	slowdownGameplayEffect = Cast<UVoidLiquidSlowdownGameplayEffect>(UVoidLiquidSlowdownGameplayEffect::StaticClass()->GetDefaultObject());
	damageGameplayEffect = Cast<UVoidLiquidDamageGameplayEffect>(UVoidLiquidDamageGameplayEffect::StaticClass()->GetDefaultObject());
	durationExtenderGameplayEffect = Cast<UVoidLiquidDurationExtenderGameplayEffect>(UVoidLiquidDurationExtenderGameplayEffect::StaticClass()->GetDefaultObject());
	magntiudeExtenderGameplayEffect = Cast<UVoidLiquidMagnitudeExtenderGameplayEffect>(UVoidLiquidMagnitudeExtenderGameplayEffect::StaticClass()->GetDefaultObject());
}

void UVoidLiquidBlockTrigger::OnOverlapStart(ABaseCharacter* const character)
{
	EMaterialTypeEnum type;
	OnEnterOverlap(type, character);
}

void UVoidLiquidBlockTrigger::OnOverlapEnd(ABaseCharacter* const character)
{
	EMaterialTypeEnum type;
	OnExitOverlap(type, character);
}

void UVoidLiquidBlockTrigger::OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	if (characterquery::is::targetable(character) && actorquery::is::alive(character))
	{
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

		FGameplayEffectQuery query;
		query.EffectDefinition = UVoidLiquidGameplayEffect::StaticClass();
		auto effects = character->GetAbilitySystemComponent()->GetActiveEffects(query);
		if (effects.Num() == 0)
		{
			FGameplayEffectSpec effectSpec(Cast<UVoidLiquidGameplayEffect>(UVoidLiquidGameplayEffect::StaticClass()->GetDefaultObject()), character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
			effectSpec.SetSetByCallerMagnitude(FName("Duration"), baseVoidTouchedDuration);
			effectSpec.SetSetByCallerMagnitude(FName("Magnitude"), baseDamageMultiplier);
			character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectSpec);
		}

		FGameplayEffectSpec effectDurationExtenderSpec(durationExtenderGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), -1);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseVoidTouchedDuration);
		effectDurationExtenderSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseDamageMultiplier);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectDurationExtenderSpec);

		FGameplayEffectSpec effectMagnitudeExtenderSpec(magntiudeExtenderGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("Duration"), -1);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseVoidTouchedDuration);
		effectMagnitudeExtenderSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseDamageMultiplier);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(effectMagnitudeExtenderSpec);

		FGameplayEffectSpec damageSpec(damageGameplayEffect, character->GetAbilitySystemComponent()->MakeEffectContext(), 1);
		character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(damageSpec);

		character->AbilitySystem->ApplyGameplayEffectSpecToSelf(FGameplayEffectSpec(slowdownGameplayEffect, character->AbilitySystem->MakeEffectContext(), 1));
	}
}

void UVoidLiquidBlockTrigger::OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const
{
	if (characterquery::is::targetable(character) && actorquery::is::alive(character))
	{
		character->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.VoidLiquid")));
		character->AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Damage.VoidLiquid")));
	}
}

bool UVoidLiquidBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	const AMobCharacter* mob = Cast<AMobCharacter>(character);
	const bool isEnderMob = mob && hasMobTag(mob->EntityType, MobTags::HashTag_Ender);
	const bool isCosmeticMob = mob && hasMobTag(mob->EntityType, MobTags::HashTag_Cosmetic);
	return Super::IsTrigger(fullBlock, character) && !isEnderMob && !isCosmeticMob;
}

UVoidLiquidGameplayEffect::UVoidLiquidGameplayEffect()  {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName("Duration");

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
	DurationMagnitude = durationMagnitude;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bDenyOverflowApplication = false;

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = FName("Magnitude");

	FGameplayModifierInfo takeDamageInfo;
	takeDamageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	takeDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	takeDamageInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(takeDamageInfo);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.VoidTouched")), 0, 1);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidTouched"));

	UIData = UVoidTouchedGameplayEffectUIData::StaticClass()->GetDefaultObject<UVoidTouchedGameplayEffectUIData>();
}

UVoidLiquidDurationExtenderGameplayEffect::UVoidLiquidDurationExtenderGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName("Duration");
	DurationMagnitude = durationMagnitude;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;

	Period = durationIncreasePeriod;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayEffectExecutionDefinition voidLiquidDurationExtenderExecution;
	voidLiquidDurationExtenderExecution.CalculationClass = UVoidLiquidDurationExtenderExecution::StaticClass();
	Executions.Add(std::move(voidLiquidDurationExtenderExecution));

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidLiquid"));
}

UVoidLiquidDurationExtenderExecution::UVoidLiquidDurationExtenderExecution()
{
	effectToApply = UVoidLiquidGameplayEffect::StaticClass();
}

void UVoidLiquidDurationExtenderExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	const auto targetAbility = ExecutionParams.GetTargetAbilitySystemComponent();
	const auto level = ExecutionParams.GetOwningSpec().GetLevel();
	const float baseDuration = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FName("BaseDuration"), false, -1.0f);
	const float baseMagnitude = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FName("BaseMagnitude"), false, -1.0f);
	FGameplayEffectSpec effectSpec(Cast<UGameplayEffect>(effectToApply->GetDefaultObject()), targetAbility->MakeEffectContext(), level);

	FGameplayEffectQuery query;
	query.EffectDefinition = effectToApply;
	auto effects = targetAbility->GetActiveEffects(query);
	if (effects.Num() > 0)
	{
		float startTime, duration;
		float magnitude = targetAbility->GetGameplayEffectMagnitude(effects[0], UHealthAttributeSet::TakeDamageMultiplierAttribute());
		targetAbility->GetGameplayEffectStartTimeAndDuration(effects[0], startTime, duration);

		effectSpec.SetSetByCallerMagnitude(FName("Duration"), FMath::Min((duration + periodIncrease) * exponetialIncrease, maxDuration));
		effectSpec.SetSetByCallerMagnitude(FName("Magnitude"), magnitude);
		targetAbility->ApplyGameplayEffectSpecToSelf(effectSpec);
	}
	else if (baseDuration > 0 && baseMagnitude > 0)
	{
		effectSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseDuration);
		effectSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseMagnitude);
		targetAbility->ApplyGameplayEffectSpecToSelf(effectSpec);
	}
}

UVoidLiquidMagnitudeExtenderGameplayEffect::UVoidLiquidMagnitudeExtenderGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName("Duration");
	DurationMagnitude = durationMagnitude;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;

	Period = magnitudeIncreasePeriod;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayEffectExecutionDefinition voidLiquidMagnitudeExtenderExecution;
	voidLiquidMagnitudeExtenderExecution.CalculationClass = UVoidLiquidMagnitudeExtenderExecution::StaticClass();
	Executions.Add(std::move(voidLiquidMagnitudeExtenderExecution));

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidLiquid"));
}

UVoidLiquidMagnitudeExtenderExecution::UVoidLiquidMagnitudeExtenderExecution()
{
	effectToApply = UVoidLiquidGameplayEffect::StaticClass();
}

void UVoidLiquidMagnitudeExtenderExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	const auto targetAbility = ExecutionParams.GetTargetAbilitySystemComponent();
	const auto level = ExecutionParams.GetOwningSpec().GetLevel();
	const float baseDuration = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FName("BaseDuration"), false, -1.0f);
	const float baseMagnitude = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FName("BaseMagnitude"), false, -1.0f);
	FGameplayEffectSpec effectSpec(Cast<UGameplayEffect>(effectToApply->GetDefaultObject()), targetAbility->MakeEffectContext(), level);

	FGameplayEffectQuery query;
	query.EffectDefinition = effectToApply;
	auto effects = targetAbility->GetActiveEffects(query);

	const auto world = targetAbility->GetWorld();
	if (effects.Num() > 0 && world)
	{
		float startTime, duration;
		float currentTime = world->GetTimeSeconds();
		float magnitude = targetAbility->GetGameplayEffectMagnitude(effects[0], UHealthAttributeSet::TakeDamageMultiplierAttribute());
		targetAbility->GetGameplayEffectStartTimeAndDuration(effects[0], startTime, duration);
		float elapsed = currentTime - startTime;

		effectSpec.SetSetByCallerMagnitude(FName("Duration"), duration - elapsed);
		effectSpec.SetSetByCallerMagnitude(FName("Magnitude"), FMath::Min((magnitude + periodIncrease * level) * exponetialIncrease, (maxMagnitude - 1)*level));
		targetAbility->ApplyGameplayEffectSpecToSelf(effectSpec);
	}
	else if(baseDuration > 0 && baseMagnitude > 0)
	{
		effectSpec.SetSetByCallerMagnitude(FName("BaseDuration"), baseDuration);
		effectSpec.SetSetByCallerMagnitude(FName("BaseMagnitude"), baseMagnitude);
		targetAbility->ApplyGameplayEffectSpecToSelf(effectSpec);
	}
}

UVoidLiquidSlowdownGameplayEffect::UVoidLiquidSlowdownGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	info.ModifierMagnitude = FScalableFloat(-slownessAmount);
	info.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.VoidLiquid.Slow"));
}

UVoidLiquidDamageGameplayEffect::UVoidLiquidDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	bExecutePeriodicEffectOnApplication = false;
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

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.VoidLiquid"));
	InheritableGameplayEffectTags.AddTag(damageTag::environmental());
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}

UVoidTouchedGameplayEffectUIData::UVoidTouchedGameplayEffectUIData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	name = NSLOCTEXT("StatusEffect", "voidtouched", "Void Touched");
	softIconClassRef = TSoftClassPtr<UUserWidget>(game::PrefabPath("UI/StatusEffects/GameplayEffect/Icons/UMG_VoidTouchedIcon.UMG_VoidTouchedIcon_C"));
	displayPreference = EStackCountDisplayPreference::AsMagnitude;
	displayMagnitudeAttribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	displayMagnitudeOffset = 1.0f;
	displayMagnitudeFractionalDigitsCount = 0;
	displayDurationUpdateFrequency = 0.5f;
}