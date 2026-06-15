#include "PushVolumeResistance.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

// ---------- GAMEPLAY EFFECTS ---------- //
UPushVolumeImmunityGameplayEffect::UPushVolumeImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume"));

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.PushVolume"));
}

UWindImmunityGameplayEffect::UWindImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume.Wind"));

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.PushVolume.Wind"));

}

UWindResistanceGameplayEffect::UWindResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.PushVolume.Wind"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::WindResistanceMagnitudeAttribute(), EGameplayModOp::Division);
}


UCurrentImmunityGameplayEffect::UCurrentImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;

	GrantedApplicationImmunityTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume.Current"));

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Immunity.PushVolume.Current"));
}

UCurrentResistanceGameplayEffect::UCurrentResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.PushVolume.Current"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::CurrentResistanceMagnitudeAttribute(), EGameplayModOp::Division);
}

// ---------- ENCHANTMENTS ---------- //
UPushVolumeImmunity::UPushVolumeImmunity() {
	TypeId = EEnchantmentTypeID::PushVolumeImmunity;
}

void UPushVolumeImmunity::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UPushVolumeImmunityGameplayEffect>(abilitySystem, 1.f);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UPushVolumeImmunity::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

// - Wind - //
UWindImmunity::UWindImmunity() {
	TypeId = EEnchantmentTypeID::WindImmunity;
}

void UWindImmunity::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UWindImmunityGameplayEffect>(abilitySystem, 1.f);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UWindImmunity::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}


UWindResistance::UWindResistance() {
	TypeId = EEnchantmentTypeID::WindResistance;

	LevelMultiplier = [this](int level) -> float {
		return Math::min(1, Resistance + PerLevelResistance * (1 - level));
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

FText UWindResistance::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEveryRoundedWordSecond(LevelMultiplier(Level))));
}

void UWindResistance::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

		float resistanceDivider = 1 / Math::max(0.01f, 1 - LevelMultiplier(Level));
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UWindResistanceGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::WindResistanceMagnitudeAttribute().GetName(), resistanceDivider);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UWindResistance::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}


// - Currents - //
UCurrentImmunity::UCurrentImmunity() {
	TypeId = EEnchantmentTypeID::CurrentImmunity;
}

void UCurrentImmunity::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UCurrentImmunityGameplayEffect>(abilitySystem, 1.f);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UCurrentImmunity::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}


UCurrentResistance::UCurrentResistance() {
	TypeId = EEnchantmentTypeID::CurrentResistance;

	LevelMultiplier = [this](int level) -> float {
		return Math::min(1, Resistance + PerLevelResistance * (1 - level));
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

FText UCurrentResistance::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEveryRoundedWordSecond(LevelMultiplier(Level))));
}

void UCurrentResistance::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

		float resistanceDivider = 1 / Math::max(0.01f, 1 - LevelMultiplier(Level));
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UCurrentResistanceGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::CurrentResistanceMagnitudeAttribute().GetName(), resistanceDivider);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UCurrentResistance::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
