#include "Dungeons.h"
#include "DifficultyModCalculation.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/difficulty/DifficultyStats.h"
#include "DungeonsGameMode.h"
#include "world/entity/MobTags.h"


UDifficultyDependantModCalculation::UDifficultyDependantModCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

FOnExternalGameplayModifierDependencyChange* UDifficultyDependantModCalculation::GetExternalModifierDependencyMulticast(const FGameplayEffectSpec& Spec, UWorld* World) const {
	if (auto&& gamemode = Cast<ADungeonsGameMode>(World->GetAuthGameMode())) {
		return &gamemode->OnDifficultyParametersChanged;
	}
	return nullptr;
}







UDifficultyMobDamageMultiplicationCalculation::UDifficultyMobDamageMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobDamageMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobDamageMultiplier();
	return difficultyMultiplier;
}



UDifficultyMobHealingMultiplicationCalculation::UDifficultyMobHealingMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobHealingMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobPerformHealingMultiplier();
	return difficultyMultiplier;
}





UDifficultyMobMaxHealthMultiplicationCalculation::UDifficultyMobMaxHealthMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobMaxHealthMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobMaxHealthMultiplier();
	return difficultyMultiplier;
}

UDifficultySpecialMobMaxHealthMultiplicationCalculation::UDifficultySpecialMobMaxHealthMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultySpecialMobMaxHealthMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetSpecialMobMaxHealthMultiplier();
	return difficultyMultiplier;
}



UDifficultyMobMaxEnduranceMultiplicationCalculation::UDifficultyMobMaxEnduranceMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobMaxEnduranceMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobMaxEnduranceMultiplier();
	return difficultyMultiplier;
}





UDifficultyMobEnduranceRecoveryMultiplicationCalculation::UDifficultyMobEnduranceRecoveryMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobEnduranceRecoveryMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobRecoverEnduranceMultiplier();
	return difficultyMultiplier;
}




UDifficultyMobSpeedMultiplicationCalculation::UDifficultyMobSpeedMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobSpeedMultiplicationCalculation::CalculateBaseMagnitude_Implementation( const FGameplayEffectSpec& Spec ) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobSpeedMultiplier();
	return difficultyMultiplier;
}




UDifficultyPlayerHealingDivisionCalculation::UDifficultyPlayerHealingDivisionCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyPlayerHealingDivisionCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetPlayerReceiveHealingMultiplier();
	return 1.f / difficultyMultiplier;
}
UDifficultyMobPushbackMultiplicationCalculation::UDifficultyMobPushbackMultiplicationCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UDifficultyMobPushbackMultiplicationCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);
	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();
	const float difficultyMultiplier = difficultyStats.GetMobPushbackMultiplier();
	return difficultyMultiplier;
}