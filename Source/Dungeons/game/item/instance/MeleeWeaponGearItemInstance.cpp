#include "Dungeons.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "MeleeWeaponGearItemInstance.h"
#include "game/abilities/effects/calculations/ItemPowerModCalculations.h"


AMeleeWeaponGearItemInstance::AMeleeWeaponGearItemInstance() {
	PowerEffects = { UMeleeDamageIncrease::StaticClass(), UMeleeHealingIncrease::StaticClass() };
}

TArray<FMeleeAttackComponentAttackVariant>& AMeleeWeaponGearItemInstance::GetEditableAttackVariants() {
	return ConfiguredAttackVariants;
}

const TArray<FMeleeAttackComponentAttackVariant>& AMeleeWeaponGearItemInstance::GetAttackVariants() const {
	return ConfiguredAttackVariants;
}

USoundCue* AMeleeWeaponGearItemInstance::GetImpactSound_Implementation(int index) const {
	if (ConfiguredAttackVariants.Num() == 0) {
		return nullptr;
	}
	return ConfiguredAttackVariants[FMath::Clamp(index, 0, ConfiguredAttackVariants.Num() - 1)].HitSoundCue;
}







/*

ITEM STAT ESTIMATES

*/

float AMeleeWeaponGearItemInstance::GetStats(EItemStats stat) const {
	return UMeleeAttackComponent::GetAttackVariantsStat(GetAttackVariants(), stat, AttackVariantResetSeconds);
}