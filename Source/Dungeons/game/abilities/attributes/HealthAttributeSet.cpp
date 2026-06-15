#include "Dungeons.h"
#include "HealthAttributeSet.h"

#include "DungeonsGameMode.h"
#include "GameplayEffectExtension.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/BackpackComponent.h"
#include "game/component/EnchantmentComponent.h"
#include "game/component/HealthComponent.h"
#include <AbilitySystemComponent.h>
#include <Net/UnrealNetwork.h>
#include "../effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/team/TeamQuery.h"

namespace DungeonsQA {

	extern TAutoConsoleVariable<int32> CVImmortal;

};

TAutoConsoleVariable<float> CVarMobStaggerMultiplier(
	TEXT("Dungeons.Mob.StaggerMultiplier"),
	0.6,
	TEXT("Set global staggering multiplier for mobs"),
	ECVF_Cheat);

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(Shield, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(Health, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(MaxHealth, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ResistDeath, UHealthAttributeSet) // D11.DB
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(TakeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(TakeMeleeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(TakeRangeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(TakeFallDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ArmorAttackItemPowerFactor, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ArmorHealingItemPowerFactor, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ReceiveHealingMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(HealthThreshold, UHealthAttributeSet)

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, ResistDeath, COND_None, REPNOTIFY_Always); // D11.DB
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, TakeDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, TakeMeleeDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, TakeRangeDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, TakeFallDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, ArmorAttackItemPowerFactor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, ArmorHealingItemPowerFactor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, ReceiveHealingMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, HealthThreshold, COND_None, REPNOTIFY_Always);
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	if (Attribute == HealthAttribute()) {
		NewValue = FMath::Min(NewValue, MaxHealth);
		if (!FMath::IsNearlyZero(ResistDeath)) {
			// D11.DB - Resist death prevents the attribute from reaching zero.
			NewValue = FMath::Max(1.0f, NewValue);
		}
		ApplyThreshold(NewValue);

	} else if (Attribute == ShieldAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
	}
	else if (Attribute == EnduranceAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, MaxEndurance);
	}

	if (DungeonsQA::CVImmortal.GetValueOnGameThread() != 0) {
		if (Attribute == HealthAttribute() && GetOwningActor()->IsA<APlayerCharacter>()) {
			NewValue = FMath::Max(NewValue, 5.0f);
		}
	}
}

void UHealthAttributeSet::ApplyThreshold(float& NewValue) const
{
	if (!FMath::IsNearlyEqual(HealthThreshold, 1.0f, 0.01f) && !FMath::IsNearlyZero(HealthThreshold, 0.01f)) {
		if (NewValue / MaxHealth < HealthThreshold && Health / MaxHealth > HealthThreshold && !FMath::IsNearlyEqual(Health / MaxHealth, HealthThreshold, 0.01f)) {
			NewValue = MaxHealth * HealthThreshold;
		}
	}
}

bool UHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) {
	if (Data.EvaluatedData.Attribute == HealthAttribute()) {
		if (Data.EvaluatedData.Magnitude < 0.f) {
			return IsDamageApplied(Data);
		}
		return IsHealingApplied(Data);
	}
	return true;
}

bool UHealthAttributeSet::IsDamageApplied(FGameplayEffectModCallbackData& data) const {
	const auto damageDealt = CalculateDamageDealt(data);
	if (damageDealt != 0) {
		ModifyDamageMagnitude(data, damageDealt);
		return true;
	}
	return false;
}

float UHealthAttributeSet::CalculateDamageDealt(FGameplayEffectModCallbackData& data) const {
	if (CanApplyDamage(data)) {
		const auto ownerCharacter = Cast<ABaseCharacter>(GetOwningActor());
		const auto damageTypeMultiplier = GetDamageTypeMultiplier(data, ownerCharacter);

		if (damageTypeMultiplier > 0 && !IsAttackMissed(data, ownerCharacter)) {
			const auto damage = -data.EvaluatedData.Magnitude * damageTypeMultiplier;

			if (ShouldSavePlayer(damage)) {
				return Health - FMath::RandRange(1.f, 0.05f * MaxHealth);
			}

			return damage;
		}
	}

	return 0.f;
}

float UHealthAttributeSet::GetDamageTypeMultiplier(FGameplayEffectModCallbackData& data, const ABaseCharacter* ownerCharacter) {
	auto damageTypeMultiplier = 0.f;
	
	if (const auto healthComponent = ownerCharacter->GetHealthComponent()) {
		FGameplayTagContainer tags;
		data.EffectSpec.GetAllAssetTags(tags);
		damageTypeMultiplier = healthComponent->GetDamageMultiplier(tags);
	}

	return damageTypeMultiplier;
}

bool UHealthAttributeSet::CanApplyDamage(FGameplayEffectModCallbackData& data) const {
	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwningActor());
	
	if (
		FMath::IsNearlyEqual(Health, 0.f) ||
		GetOwningActor()->IsPendingKill() ||
		!ownerCharacter->bCanBeDamaged
	) {
		return false;
	}

	if (const auto* healthComponent = ownerCharacter->GetHealthComponent()) {
		if (healthComponent->Invincible) {
			return false;
		}
	}

	if (const auto* player = Cast<APlayerCharacter>(ownerCharacter)) {
		if (player->IsImmortal()) {
			return false;
		}
	}

	return CanDamageHurtFriends(data) || CanApplyDamageToTarget(data);
}

bool UHealthAttributeSet::CanDamageHurtFriends(const FGameplayEffectModCallbackData& data) {
	const auto sourceTags = data.EffectSpec.CapturedSourceTags.GetAggregatedTags();	
	return sourceTags->HasTag(damageTag::damageFriends()) || data.EffectSpec.DynamicAssetTags.HasTag(damageTag::damageFriends());
}

bool UHealthAttributeSet::CanApplyDamageToTarget(const FGameplayEffectModCallbackData& data) const {
	const auto ownerCharacter = Cast<ABaseCharacter>(GetOwningActor());

	const auto context = effects::GetDungeonsContextFromSpec(data.EffectSpec);
	return teamquery::can::damage(ownerCharacter->GetCurrentTeam(), context->InstigatorTeam);
}

bool UHealthAttributeSet::IsAttackMissed(FGameplayEffectModCallbackData& data, const ABaseCharacter* ownerCharacter) {
	auto missedAttack = false;

	const auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

	DungeonsGearUtilLibrary::OnBeforeDamageReceived(ownerCharacter, missedAttack, data, abilitySystem);

	return missedAttack;
}

bool UHealthAttributeSet::ShouldSavePlayer(const float damage) const {
	if (GetOwningActor()->IsA<APlayerCharacter>()) {
		const auto saveThreshold = 120.0f;
		return Health > saveThreshold / 2
			&& damage > Health
			&& damage > saveThreshold
			&& Health + 2 * saveThreshold > damage
			&& FMath::FRand() < 0.5f;
	}
	return false;
}

void UHealthAttributeSet::ModifyDamageMagnitude(FGameplayEffectModCallbackData& data, const float healthReduction) const {
	data.EvaluatedData.Magnitude = -healthReduction;

	if (data.EvaluatedData.Magnitude < 0.f) {
		if (const auto backpackComponent = GetOwningActor()->FindComponentByClass<UMochilaComponent>()) {
			backpackComponent->ApplyDamage(data.EffectSpec);
		}

		if (auto gameMode = Cast<ADungeonsGameMode>(GetOwningActor()->GetWorld()->GetAuthGameMode())) {
			const auto instigator = data.EffectSpec.GetContext().GetInstigator();
			gameMode->OnActorDamage(GetOwningActor(), -(data.EvaluatedData.Magnitude), instigator, data.EffectSpec.GetContext().GetEffectCauser());
		}
	}

	if (Shield > 0.0f) {
		data.EvaluatedData.Attribute = ShieldAttribute();
	}
}

bool UHealthAttributeSet::IsHealingApplied(FGameplayEffectModCallbackData& data) const {
	if (Health <= 0.f) {
		return HasReviveTag(data);
	}

	return true;
}

bool UHealthAttributeSet::HasReviveTag(FGameplayEffectModCallbackData& data) {
	FGameplayTagContainer tags;
	data.EffectSpec.GetAllAssetTags(tags);
	return tags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Revive")));
}

void UHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const {
	if (Attribute == HealthAttribute()) {
		NewValue = FMath::Min(NewValue, MaxHealth);
		ApplyThreshold(NewValue);
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) {
	ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwningActor());
	if (Data.EvaluatedData.Magnitude < 0 && Data.EvaluatedData.Attribute == HealthAttribute()) { // damage
		auto abilitySystem = GetOwningAbilitySystemComponent();

		DungeonsGearUtilLibrary::OnAfterReceivedDamage(ownerCharacter, Data);

		FGameplayTagContainer tags;
		Data.EffectSpec.GetAllAssetTags(tags);
		const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Data.EffectSpec);
		if (!tags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")))) {
			const float enduranceDamage = Data.EvaluatedData.Magnitude * context->StunMultiplier * StaggerMultiplier * CVarMobStaggerMultiplier.GetValueOnGameThread();
			if (!FMath::IsNearlyZero(enduranceDamage)) {
				abilitySystem->ApplyModToAttributeUnsafe(EnduranceAttribute(), EGameplayModOp::Additive, enduranceDamage);
			}
		}
	}
}

DEFINE_ATTRIBUTE_FUNCTION(Shield, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(Health, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(MaxHealth, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ResistDeath, UHealthAttributeSet) // D11.DB
DEFINE_ATTRIBUTE_FUNCTION(TakeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(TakeMeleeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(TakeRangeDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(TakeFallDamageMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ArmorAttackItemPowerFactor, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ArmorHealingItemPowerFactor, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(Endurance, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(MaxEndurance, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(EnduranceRecoveryPerSecond, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(StaggerMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ReceiveHealingMultiplier, UHealthAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(HealthThreshold, UHealthAttributeSet)