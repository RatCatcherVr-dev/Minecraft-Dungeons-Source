#include "Dungeons.h"
#include "game/item/power/ItemPowerEffect.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerFormat.h"
#include "ItemPowerEffectDefs.h"
#include "game/util/ValueFormat.h"
#include "util/CollectionUtils.h"
#include "game/abilities/effects/ItemPowerGameplayEffects.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/item/instance/ArmorGearItemInstance.h"
#include "game/component/RangedAttackComponent.h"
#include "ItemPowerUtil.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "ItemPowerEffect"

UDamageIncrease::UDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
	PowerEffectTemplate = LOCTEXT("DamageIncrease", "{0} damage");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamage", "{0} damage");	

}
UMeleeDamageIncrease::UMeleeDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;	
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
	PowerEffectTemplate = LOCTEXT("DamageIncreaseMelee", "{0} melee damage");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamageMelee", "{0} melee damage");
	GameplayEffect = UMeleeDamageItemPowerGameplayEffect::StaticClass();
}

URangedDamageIncrease::URangedDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
	PowerEffectTemplate = LOCTEXT("DamageIncreaseRanged", "{0} ranged damage");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamageRanged", "{0} ranged damage");
	GameplayEffect = URangedDamageItemPowerGameplayEffect::StaticClass();
}


UItemRangedDamageIncrease::UItemRangedDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
	PowerEffectTemplate = LOCTEXT("DamageIncreaseRanged", "{0} ranged damage");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamageRanged", "{0} ranged damage");
}

UItemArtifactDamageIncrease::UItemArtifactDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
	PowerEffectTemplate = LOCTEXT("DamageIncreaseArtifact", "{0} artifact damage");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamageArtifact", "{0} artifact damage");
}

UItemDamagePerSecondIncrease::UItemDamagePerSecondIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamagePerSecond;
	PowerEffectTemplate = LOCTEXT("DamageIncreasePerSecond", "{0} damage per second");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamagePerSecond", "{0} damage per second");
}


UItemRangedDamagePerSecondIncrease::UItemRangedDamagePerSecondIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamagePerSecond;
	PowerEffectTemplate = LOCTEXT("DamageIncreasePerSecondRanged", "{0} ranged damage per second");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamagePerSecondRanged", "{0} ranged damage per second");
}

UItemArtifactDamagePerSecondIncrease::UItemArtifactDamagePerSecondIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamagePerSecond;
	PowerEffectTemplate = LOCTEXT("DamageIncreasePerSecondArtifact", "{0} artifact damage per second");
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamagePerSecondArtifact", "{0} artifact damage per second");
}


UMaxHealthIncrease::UMaxHealthIncrease() {
	ItemPowerFunc = game::item::power::MaxHealthMultiplier;
	PowerEffectTemplate = LOCTEXT("MaxHealthIncrease", "{0} increased health");
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;	
	PowerEffectAbsoluteTemplate = LOCTEXT("xHealth", "{0} health");
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteMaxHealthIncrease;
	GameplayEffect = UMaxHealthItemPowerGameplayEffect::StaticClass();
}


UArmorDamageIncrease::UArmorDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerEffectTemplate = LOCTEXT("DamageIncrease", "{0} damage");
	HiddenFromPlayer = true;
	GameplayEffect = UArmorDamageItemPowerGameplayEffect::StaticClass();
}


UDamageReduction::UDamageReduction() {
	ItemPowerFunc = game::item::power::DamageReductionMultiplier;
	PowerEffectTemplate = LOCTEXT("DamageReduction", "{0} damage taken");	
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}


UDurationIncrease::UDurationIncrease() {
	ItemPowerFunc = game::item::power::DurationIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("DurationIncrease", "{0} duration");
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier){
		return valueformat::asDurationSingleDecimalSecond(instance->GetItemType().getDurationSeconds()*multiplier);
	};
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

UDamageBoosted::UDamageBoosted() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("DamageBoosted", "{0} boosted damage");
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier){
		return valueformat::asRelativeMultiplierPercentageChange(instance->GetStats(EItemStats::DamageBoost)*multiplier);
	};
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

UHealingIncrease::UHealingIncrease() {
	ItemPowerFunc = game::item::power::HealingIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("HealingIncrease", "{0} healing");
	PowerEffectAbsoluteTemplate = LOCTEXT("xHealthHealed", "{0} health healed");
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteHealthHealed;
}

UMeleeHealingIncrease::UMeleeHealingIncrease() {	
	GameplayEffect = UMeleeHealingItemPowerGameplayEffect::StaticClass();
	HiddenFromPlayer = true;
}

URangedHealingIncrease::URangedHealingIncrease() {
	GameplayEffect = URangedHealingItemPowerGameplayEffect::StaticClass();
	HiddenFromPlayer = true;
}

UArmorHealingIncrease::UArmorHealingIncrease() {
	HiddenFromPlayer = true;
	GameplayEffect = UArmorHealingItemPowerGameplayEffect::StaticClass();
}


UStunDurationIncrease::UStunDurationIncrease() {
	ItemPowerFunc = game::item::power::StunDurationIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("StunDurationIncrease", "{0} stun duration");
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {
		const auto duration = instance->GetStats(EItemStats::StunDuration)*multiplier;
		if (duration > 10.f){
			return valueformat::asDurationRoundedSecond(duration);
		} else {
			return valueformat::asDurationSingleDecimalSecond(duration);
		}
	};
}

UPushForceIncrease::UPushForceIncrease() {
	ItemPowerFunc = game::item::power::ForceIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("PushForceIncrease", "{0} push force");
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerEffectAbsoluteTemplate = LOCTEXT("xBlocksPushed", "{0} blocks pushed");
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {
		return valueformat::asConstant(instance->GetStats(EItemStats::BlocksPushed)*multiplier);
	};
}

USummonDamageIncrease::USummonDamageIncrease() {
	ItemPowerFunc = game::item::power::DamageIncreaseMultiplier;
	PowerEffectTemplate = LOCTEXT("SummonDamageIncrease", "{0} summon damage");	
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerEffectAbsoluteTemplate = LOCTEXT("xDamageSummon", "{0} summon damage");
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteDamage;
}

UCooldownReduction::UCooldownReduction() {
	ItemPowerFunc = game::item::power::CooldownReductionMultiplier;
	PowerEffectTemplate = LOCTEXT("CooldownReduction", "{0} cooldown");
	//PowerEffectAbsoluteTemplate = LOCTEXT("xCooldownTemplate", "{0} cooldown");
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	//PowerMultiplierAbsoluteFormatter = [this](const AItemInstance* instance, float multiplier) {
	//	return valueformat::asDurationSingleDecimalSecond(instance->GetItemType().getCoolDownSeconds()*multiplier);
	//};
}

UItemHealthIncrease::UItemHealthIncrease() {
	ItemPowerFunc = game::item::power::MaxHealthMultiplier;
	PowerMultiplierFormatter = valueformat::asDamage;
	PowerMultiplierAbsoluteFormatter = game::item::power::format::AbsoluteItemHealth;
	PowerEffectTemplate = LOCTEXT("HealthIncrease", "{0} health");
	PowerEffectAbsoluteTemplate = LOCTEXT("xHealth", "{0} health");
}

USpeedIncrease::USpeedIncrease() {
	ItemPowerFunc = game::item::power::HiddenSpeedIncreaseMultiplier;
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	PowerEffectTemplate = LOCTEXT("SpeedIncrease", "{0} speed");
	PowerEffectAbsoluteTemplate = LOCTEXT("xSpeed", "{0} speed");
}


UItemPowerAsFlatDamageBoost::UItemPowerAsFlatDamageBoost() {
	ItemPowerFunc = [](float power) { return 1.f + (power <= 1.f ? 0.f : (UItemPowerUtil::GetItemPowerDisplayValueFloat(power) / 100.f)); };
	PowerEffectTemplate = LOCTEXT("DamageBoosted", "{0} boosted damage");
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {
		float newMultiplier = 1.f + ((multiplier - 1.f) * instance->GetStats(EItemStats::DamageBoost));
		return valueformat::asRelativeMultiplierPercentageChange(newMultiplier);
	};
	PowerMultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

UQuiverAmmoIncrease::UQuiverAmmoIncrease() {
	ItemPowerFunc = game::item::power::HiddenQuiverAmmoIncrease;
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {
		float newValue = instance->GetStats(EItemStats::QuiverAmmo) + multiplier;
		return valueformat::asRoundedConstant(newValue);
	};
	PowerEffectTemplate = LOCTEXT("QuiverAmmoIncrease", "{0} arrows");
	PowerEffectAbsoluteTemplate = LOCTEXT("xQuiverAmmoIncrease", "{0} arrows");
}

UDoubleItemChanceIncrease::UDoubleItemChanceIncrease() {
	// 26.5 is our current max level
	ItemPowerFunc = [](float power) { return UKismetMathLibrary::MapRangeClamped(power, 1, 26.5f, 0.f, .8f); };
	PowerEffectTemplate = LOCTEXT("DoubleItemIncrease", "{0} double item chance");
	PowerMultiplierAbsoluteFormatter = [this](const ABaseCharacter& owner, const AItemInstance* instance, float multiplier) {
		return valueformat::asPercentage(multiplier);
	};
	PowerMultiplierFormatter = valueformat::asPercentage;
}
#undef LOCTEXT_NAMESPACE
