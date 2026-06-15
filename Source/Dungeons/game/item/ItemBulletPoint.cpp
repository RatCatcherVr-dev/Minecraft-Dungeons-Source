#include "Dungeons.h"
#include "ItemBulletPoint.h"
#include "game/util/ValueFormat.h"
#include "power/ItemPowerUtil.h"

EBulletIcon FItemBulletPoint::IconFromRarity(EItemRarity Rarity)
{
	switch (Rarity) {
	case EItemRarity::Common:
		return EBulletIcon::Common;		
	case EItemRarity::Rare:		
		return EBulletIcon::Rare;
	case EItemRarity::Unique:
		return EBulletIcon::Unique;
	default:
		return EBulletIcon::Generic;
	}
}



#define LOCTEXT_NAMESPACE "ItemType"

FItemBulletPoint FItemBulletPoint::CreateTargetsMaxiumumBulletPoint(int targets) {
	if (targets == 1) {
		return { FText::Format(LOCTEXT("max_x_target", "{0} target maximum"), FText::FromString(valueformat::asConstant(targets))), EBulletIcon::Generic };
	} else {
		return { FText::Format(LOCTEXT("max_x_targets", "{0} targets maximum"), FText::FromString(valueformat::asConstant(targets))), EBulletIcon::Generic };
	}
}

FItemBulletPoint FItemBulletPoint::CreateSoulGatherBulletPoint(int soulGatherCount) {
	const auto displayValue = valueformat::asRelativeConstant(soulGatherCount);
	return { FText::Format(LOCTEXT("x_soul_gathering", "{0} Soul Gathering"), FText::FromString(displayValue)), EBulletIcon::SoulGather };
}

FItemBulletPoint FItemBulletPoint::CreateCooldownBulletPoint(float cooldownSeconds) {
	const auto secondsText = (cooldownSeconds == FMath::RoundToFloat(cooldownSeconds) ? valueformat::asDurationRoundedSecond(cooldownSeconds) : valueformat::asDurationSingleDecimalSecond(cooldownSeconds));
	return { FText::Format(LOCTEXT("x_cooldown_template", "{0} cooldown"), FText::FromString(secondsText)), EBulletIcon::Cooldown };
}

FItemBulletPoint FItemBulletPoint::CreateCharacteristicBulletPoint(const FItemCharacteristic& characteristic) {
	return FItemBulletPoint(characteristic.Text, characteristic.bIsNetherite ? EBulletIcon::Netherite : FItemBulletPoint::IconFromRarity(characteristic.Rarity), characteristic.mEnchantment.Get({}));
}

FText FItemBulletPoint::GetItemPowerRangeText(const game::item::generator::PowerRange& itemPowerRange) {
	return FText::Format(INVTEXT("{0}-{1}"), UItemPowerUtil::GetItemPowerDisplayText(itemPowerRange.min()), UItemPowerUtil::GetItemPowerDisplayText(itemPowerRange.max()));
}

FItemBulletPoint FItemBulletPoint::CreateEstimatedItemPowerRangeBulletPoint(const game::item::generator::PowerRange& itemPowerRange) {
	const FText powerRangeText = FText::Format(LOCTEXT("itempowerrange_estimated_template", "Estimated Item power: {0}"), FItemBulletPoint::GetItemPowerRangeText(itemPowerRange));
	return { powerRangeText, EBulletIcon::Generic };
}

FItemBulletPoint FItemBulletPoint::CreateUpgradedItemPowerRangeBulletPoint(const game::item::generator::PowerRange& itemPowerRange) {
	const FText powerRangeText = FText::Format(LOCTEXT("itempowerrange_powerafterupgrade_template", "Item power after upgrade: {0}"), FItemBulletPoint::GetItemPowerRangeText(itemPowerRange));
	return { powerRangeText, EBulletIcon::Generic };
}

FItemBulletPoint FItemBulletPoint::CreateSeasonalLimitedBulletPoint() {
	const FText seasonalDesc = LOCTEXT("special_event_item", "Special event item");
	return { seasonalDesc, EBulletIcon::SeasonalLimited };
}

#undef LOCTEXT_NAMESPACE

//Outside the namespace because i stole this loc-text from another namespace.
FItemBulletPoint FItemBulletPoint::CreateItemPowerRangeBulletPoint(const game::item::generator::PowerRange& itemPowerRange) {
	const FText powerRangeText = FText::Format(NSLOCTEXT("items", "mysterybox_itempowerrange_template", "Item power: {0}-{1}"), UItemPowerUtil::GetItemPowerDisplayText(itemPowerRange.min()), UItemPowerUtil::GetItemPowerDisplayText(itemPowerRange.max()));
	return { powerRangeText, EBulletIcon::Generic };
}

FItemBulletPoint FItemBulletPoint::CreateLabeledCounterBulletPoint(const FText& label, int current, int max) {	
	return { valueformat::asCounterLabeled(label, current, max), EBulletIcon::Generic };
}

FItemBulletPoint FItemBulletPoint::CreateLabeledCounterBulletPoint(const FText& label, int current) {
	return { valueformat::asCounterLabeled(label, current), EBulletIcon::Generic };
}

FText FItemBulletPoint::GetAnyAncientChanceText(float anyChance) {
	return FText::Format(NSLOCTEXT("hypermission", "xChanceToEncounterAnyAncient", "{0} Chance to encounter any Ancient"), FText::FromString(valueformat::asPercentageChance(anyChance)));
}

FText FItemBulletPoint::GetAverageAncientsText(float average) {
	return FText::Format(NSLOCTEXT("hypermission", "xAncientsOnAverage", "{0} Ancients on average"), FText::FromString(valueformat::asSingleDecimalConstant(average)));
}

FItemBulletPoint FItemBulletPoint::CreateAnyAncientChanceBulletPoint(float anyChance) {
	return { GetAnyAncientChanceText(anyChance), EBulletIcon::Common };
}

FItemBulletPoint FItemBulletPoint::CreateAverageAncientsBulletPoint(float average) {
	return { GetAverageAncientsText(average), EBulletIcon::Common };
}


