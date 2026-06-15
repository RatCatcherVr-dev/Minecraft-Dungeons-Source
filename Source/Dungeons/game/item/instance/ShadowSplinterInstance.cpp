#include "Dungeons.h"
#include "ShadowSplinterInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/component/SoulComponent.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName ShadowSplinterEffectDuration(TEXT("ShadowSplinterDuration"));
}

AShadowSplinterInstance::AShadowSplinterInstance() {
	Effect = UShadowSplinterGameplayEffect::StaticClass();
	PowerEffects = { UDurationIncrease::StaticClass() };
	SharedPassiveCooldown = 0.f;
	bHasManualCooldownActivation = true;
}

int AShadowSplinterInstance::GetDisplayCount() const {
	return 0;
}

void AShadowSplinterInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);
	if (HasAuthority()) {
		auto owner = GetOwner();
		auto playerOwner = Cast<APlayerCharacter>(owner);
		PredictionKey = predictionKey;
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec(Cast<UShadowSplinterGameplayEffect>(Effect->GetDefaultObject()), OwnerAbilitySystem->MakeEffectContext(), ItemPower);

		const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
		spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::ShadowSplinter.getDurationSeconds() * ItemPowerMultiplier);
		spec.SetSetByCallerMagnitude(TEXT("Magnitude"), MeleePowerBoostAmount);
		Handle = OwnerAbilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

		playerOwner->OnInvisibilityGained();

		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandler, [=]() {
			CooldownTimerHandler.Invalidate();
			Cooldown().TriggerCooldown(game::item::type::ShadowSplinter.getDurationSeconds(), predictionKey);
		},
			game::item::type::ShadowSplinter.getDurationSeconds(), false);
	}
}

void AShadowSplinterInstance::BeginPlay()
{
	Super::BeginPlay();

	if (auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		OwnerAbilitySystem = playerOwner->GetAbilitySystemComponent();
		OwnerAbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility")).AddUObject(this, &AShadowSplinterInstance::OnInvisibilityChanged);
	}
}

void AShadowSplinterInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandler);
}

bool AShadowSplinterInstance::CanActivate() const
{
	return Super::CanActivate() && OwnerAbilitySystem->GetTagCount(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility")) <= 0;
}

float AShadowSplinterInstance::GetSoulActivationCost() const
{
	const auto& itemType = GetItemType();
	float soulCost = itemType.getSoulCost();
	if (soulCost < 1) {
		if (APlayerCharacter* OwnerAsPlayer = Cast<APlayerCharacter>(GetOwner())) {
			if (USoulComponent* OwnerSoulComponent = OwnerAsPlayer->FindComponentByClass<USoulComponent>()) {
				return OwnerSoulComponent->GetMaxSoulCount() * soulCost;
			}
		}
	}
	return soulCost;
}

void AShadowSplinterInstance::RemoveEquippedEffects()
{
	if (Handle.IsValid() && CooldownTimerHandler.IsValid()) {
		TriggerCooldown();
		OwnerAbilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

void AShadowSplinterInstance::OnInvisibilityChanged(const FGameplayTag tag, const int32 tagCount) {
	if (tagCount <= 0 && CooldownTimerHandler.IsValid()) {
		TriggerCooldown();
	}
}

void AShadowSplinterInstance::TriggerCooldown()
{
	Cooldown().TriggerCooldown(game::item::type::ShadowSplinter.getDurationSeconds(), PredictionKey);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandler);
}

UShadowSplinterGameplayEffect::UShadowSplinterGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = TEXT("Magnitude");
	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;

	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Invisibility"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ShadowSplinter"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Buff.Backstab"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Invisible"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("AI.Ignore"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}
