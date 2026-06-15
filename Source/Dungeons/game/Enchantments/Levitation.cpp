#include "Levitation.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/util/ActorQuery.h"
#include "util/CharacterQuery.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "world/entity/MobTags.h"
#include "AbilitySystemGlobals.h"

const FName ULevitationGameplayEffect::EffectMagnitudeName = FName("EffectMagnitude");
const FName ULevitationFallGameplayEffect::FallMultiplierName = FName("FallDamageTaken");

ULevitationGameplayEffect::ULevitationGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;

	FGameplayModifierInfo infoGravity;
	infoGravity.Attribute = UMovementAttributeSet::GravityAttribute();

	FSetByCallerFloat ScalableFloatGravity;
	ScalableFloatGravity.DataName = ULevitationGameplayEffect::EffectMagnitudeName;

	FGameplayEffectModifierMagnitude ModifierGravityMagnitude(ScalableFloatGravity);
	infoGravity.ModifierMagnitude = ModifierGravityMagnitude;
	infoGravity.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(infoGravity);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Levitation"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Levitation"));
}

ULevitation::ULevitation()
{

}

void ULevitation::AfterDealtDamage(AActor* toWhat)
{
	ABaseCharacter* target = Cast<ABaseCharacter>(toWhat);

	if ((!CanActivate && !bAlwaysTrigger) || !target || !characterquery::is::targetable(target) || !actorquery::is::alive(target)) return;

	if (auto mob = Cast<AMobCharacter>(toWhat))
	{
		if (hasMobTag(mob->EntityType, MobTags::HashTag_Miniboss) ||
			hasMobTag(mob->EntityType, MobTags::HashTag_Ancient) ||
			!characterquery::is::movable(mob))
		{
			return;
		}
	}

	bool isUnderwater = target->IsUnderwater();

	CanActivate = false;

	const float EffectDuration = Duration;

	UAbilitySystemComponent* abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

	// Add pushbacks
	FPushback push;
	push.pushbackStrength = LaunchStrength;
	if (isUnderwater)
	{
		push.pushbackStrength *= 3.5;
	}
	push.enablePushback = true;
	pushback::pushback(push, FVector(0, 0, 0), *toWhat, 1.0f, true, true);

	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<ULevitationGameplayEffect>(abilitySystem, Level);

	spec.SetSetByCallerMagnitude(effects::DurationName, EffectDuration);
	spec.SetSetByCallerMagnitude(ULevitationGameplayEffect::EffectMagnitudeName, -EffectStrength);

	auto handle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent());

	if (handle.WasSuccessfullyApplied())
	{
		FGameplayEffectSpec FallSpec = effects::CreateGameplayEffectSpec<ULevitationFallGameplayEffect>(abilitySystem, 1);
		FallSpec.SetSetByCallerMagnitude(effects::DurationName, EffectDuration * 2);
		FallSpec.SetSetByCallerMagnitude(ULevitationGameplayEffect::EffectMagnitudeName, Level * FallDamagePercentagePerLevel);
		abilitySystem->ApplyGameplayEffectSpecToTarget(FallSpec, target->GetAbilitySystemComponent());
	}

	FGameplayEffectSpec ImmunitySpec = effects::CreateGameplayEffectSpec<ULevitationImmunityGameplayEffect>(abilitySystem, 1);
	ImmunitySpec.SetSetByCallerMagnitude(effects::DurationName, EffectDuration * 2);
	abilitySystem->ApplyGameplayEffectSpecToTarget(ImmunitySpec, target->GetAbilitySystemComponent());

	abilitySystem->RemoveActiveGameplayEffect(LevitationReadyEffectHandle);

	if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
}

void ULevitation::OnDodgeRollEnd(FPredictionKey key)
{
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, 1.0f);
	LevitationReadyEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);

	CanActivate = true;
}

void ULevitation::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (CanActivate) {
		GetCharacterOwner()->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(LevitationReadyEffectHandle);
		CanActivate = false;
	}

	Super::EndPlay(EndPlayReason);
}

ULevitationMelee::ULevitationMelee()
{
	PredictiveExecution = true;
	MultiplierFormatter = valueformat::asPercentage;

}

void ULevitationMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window)
{
	AfterDealtDamage(toWhat);
}

ULevitationRanged::ULevitationRanged()
{
	TypeId = EEnchantmentTypeID::LevitationShot;
	PredictiveExecution = true;
	MultiplierFormatter = valueformat::asPercentage;

}

void ULevitationRanged::OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) 
{
	if(CanActivate)
	{
		fromProjectile->SetPushbackMultiplier(0.01);
	}
}

void ULevitationRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream)
{
	AfterDealtDamage(toWhat);
}


ULevitationImmunityGameplayEffect::ULevitationImmunityGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Levitation"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.Levitation"));

	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;

	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Levitation"), 0, 1);
}

ULevitationFallGameplayEffect::ULevitationFallGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.LevitationFalling"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.LevitationFalling"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned"));
}

ULevitationReadyGameplayEffect::ULevitationReadyGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
}