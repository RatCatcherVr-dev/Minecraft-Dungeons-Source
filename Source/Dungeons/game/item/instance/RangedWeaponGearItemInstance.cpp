#include "Dungeons.h"
#include "RangedWeaponGearItemInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/effects/calculations/ItemPowerModCalculations.h"

ARangedWeaponGearItemInstance::ARangedWeaponGearItemInstance() {
	PowerEffects = { URangedDamageIncrease::StaticClass(), URangedHealingIncrease::StaticClass() };

	ChargeShootEffectTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Bow.ChargeShot");
}

TSubclassOf<ABaseProjectile> ARangedWeaponGearItemInstance::GetProjectileOverrideForItemType(const FItemId& itemType) const {
	auto* projectileSubclass = ProjectileOverrides.FindByPredicate([&itemType](const TSubclassOf<ABaseProjectile>& subclass) {
		return subclass->GetDefaultObject<ABaseProjectile>()->GetProjectileItemType() == itemType;
	});

	return projectileSubclass? *projectileSubclass: nullptr;
}

int ARangedWeaponGearItemInstance::GetProjectileStackCount() const {
	return ProjectileStackCount;
}

float ARangedWeaponGearItemInstance::GetStats(EItemStats stat) const {

	auto overrideProjecileClass = GetProjectileOverrideForItemType(game::item::type::Arrow.getId());	
	switch (stat) {
	case EItemStats::DamagePerQuiver:
	{
		return URangedAttackComponent::GetRangedAttackStat(AttackDefintion, overrideProjecileClass, EItemStats::DamagePerVolley) * ProjectileStackCount;
	}
	case EItemStats::QuiverAmmo:
	{
		return ProjectileStackCount;
	}
	default:
		return URangedAttackComponent::GetRangedAttackStat(AttackDefintion, overrideProjecileClass, stat);
	}
}

RangedWeaponType ARangedWeaponGearItemInstance::GetRangedWeaponType() const
{
	return GetItemType().getRangedWeaponType();
}

FGameplayTag ARangedWeaponGearItemInstance::GetChargeShootEffectTag() const {
	return ChargeShootEffectTag;
}

USoundCue* ARangedWeaponGearItemInstance::GetAnimNotifyStateSound(FString stateName) const {
	if (const auto animNotifyDataPtr = AnimNotifyStateData.FindByPredicate([stateName](const FAnimNotifyStateData& notifyData) { return stateName.Contains(notifyData.NotifyStateName); })) {
		return animNotifyDataPtr->TriggerSound;
	}
	return nullptr;
}

FAnimNotifyStateData::FAnimNotifyStateData()
	: TriggerSound(nullptr) {
}

FAnimNotifyStateData::FAnimNotifyStateData(FString notifyStateName, USoundCue* sound)
	: NotifyStateName(std::move(notifyStateName))
	, TriggerSound(sound) {
}
