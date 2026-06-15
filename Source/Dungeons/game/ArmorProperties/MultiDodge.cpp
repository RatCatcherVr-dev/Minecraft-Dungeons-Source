#include "Dungeons.h"
#include "MultiDodge.h"
#include "game/Enchantments/Enchantment.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/component/PlayerCharacterMovementComponent.h"

namespace DodgeCharges {
	FName DodgeChargesName(TEXT("DodgeCharges"));
	FName MaxDodgeChargesName(TEXT("MaxDodgeCharges"));
}

UMultiDodgeMaxDodgesGameplayEffect::UMultiDodgeMaxDodgesGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo DodgeChargesInfo;
	DodgeChargesInfo.Attribute = UMovementAttributeSet::MaxDodgeChargesAttribute();

	FSetByCallerFloat DodgeChargesMagnitude;
	DodgeChargesMagnitude.DataName = DodgeCharges::MaxDodgeChargesName;

	DodgeChargesInfo.ModifierMagnitude = DodgeChargesMagnitude;
	DodgeChargesInfo.ModifierOp = EGameplayModOp::Type::Override;

	Modifiers.Add(DodgeChargesInfo);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiDodge"));
}

UMultiDodgeIncreamentDodgeGameplayEffect::UMultiDodgeIncreamentDodgeGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo DodgeChargesInfo;
	DodgeChargesInfo.Attribute = UMovementAttributeSet::DodgeChargesAttribute();

	FSetByCallerFloat DodgeChargesMagnitude;
	DodgeChargesMagnitude.DataName = DodgeCharges::DodgeChargesName;

	DodgeChargesInfo.ModifierMagnitude = DodgeChargesMagnitude;
	DodgeChargesInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(DodgeChargesInfo);
}

UMultiDodge::UMultiDodge()
{
	TypeId = EEnchantmentTypeID::MultiDodge;

	LevelMultiplier = [this](int level) -> int {
		return 1 + level;
	};
	MultiplierFormatter = valueformat::asConstant;
}

void UMultiDodge::AfterDodgeCooldown()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (!HasMaxDodgeCharges())
		{
			CreateAndApplyIncrementDodgeEffect();
		}
	}
}

void UMultiDodge::CreateAndApplyMaxDodgeEffect()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		auto abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

		if (abilitySystem)
		{
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UMultiDodgeMaxDodgesGameplayEffect>(abilitySystem);
			spec.SetSetByCallerMagnitude("MaxDodgeCharges", LevelMultiplier(Level));
			Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

void UMultiDodge::RemoveDodgeEffect()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		auto abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

		if (abilitySystem)
		{
			abilitySystem->RemoveActiveGameplayEffect(Handle);
		}
	}
}

void UMultiDodge::CreateAndApplyIncrementDodgeEffect()
{
	UDungeonsEffectLibrary::HitFlashOnActor(GetOwner(), FLinearColor(0.0515938f, 2.7589188f, 25.0f, 1.0f), 0.15f);

	if (GetOwnerRole() == ROLE_Authority)
	{
		auto abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

		if (abilitySystem)
		{
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UMultiDodgeIncreamentDodgeGameplayEffect>(abilitySystem);
			spec.SetSetByCallerMagnitude("DodgeCharges", 1);
			spec.CapturedSourceTags = FTagContainerAggregator();
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

bool UMultiDodge::HasMaxDodgeCharges()
{
	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

	if (abilitySystem)
	{
		return abilitySystem->GetNumericAttribute(UMovementAttributeSet::DodgeChargesAttribute()) >= LevelMultiplier(Level);
	}
	return false;
}

void UMultiDodge::OnCooldownChanged(bool nowOnCooldown)
{
	nowOnCooldown ? OnDodgeCounterDecreased() : OnDodgeCounterIncreased();
	if (!nowOnCooldown)
	{
		AfterDodgeCooldown();
	}
}

FText UMultiDodge::CreateDescription() const
{
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asConstantWord(LevelMultiplier(Level))));
}

void UMultiDodge::OnStart()
{
	CreateAndApplyMaxDodgeEffect();

	if (APlayerCharacter* OurCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		OurCharacter->GetPlayerCharacterMovementComponent()->ApplyDodgeCooldown(GetAbilitySystemComponent());
		OurCharacter->OnPlayerDodgeRollingCooldownChanged.AddUniqueDynamic(this, &UMultiDodge::OnCooldownChanged);
	}
}

void UMultiDodge::BeginPlay() {
	Super::BeginPlay();
}

void UMultiDodge::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;

	RemoveDodgeEffect();
}

