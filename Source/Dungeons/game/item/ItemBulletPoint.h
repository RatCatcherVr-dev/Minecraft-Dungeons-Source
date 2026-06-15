#pragma once
#include "UObject/ObjectMacros.h"
#include "ItemRarity.h"
#include "ItemTypeDefs.h"
#include "ItemCharacteristic.h"
#include "generator/ItemGeneratorTypes.h"
#include "enchantment/EnchantmentData.h"
#include "ItemBulletPoint.generated.h"


UENUM(BlueprintType)
enum class EBulletIcon : uint8 {
	Generic,
	ItemPower,	
	SoulGather,
	SoulUse,
	Cooldown,
	Common,
	Rare,
	Unique,
	Netherite,
	SeasonalLimited,
	EnchantmentPoint,
	Archetype,	
	Gold,
	ExperienceLevel,
};
ENUM_NAME(EBulletIcon);



USTRUCT(BlueprintType)
struct DUNGEONS_API FItemBulletPoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EBulletIcon Icon = EBulletIcon::Generic;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	ItemTag TagIcon = ItemTag::Unset;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	bool Upgraded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FEnchantmentData Enchantment;

	FItemBulletPoint(FText Text, EBulletIcon Icon, ItemTag TagIcon, bool Upgraded, const FEnchantmentData& Enchantment)
		: Text(Text)
		, Icon(Icon)
		, TagIcon(TagIcon)
		, Upgraded(Upgraded)
		, Enchantment(Enchantment) {
	}

	FItemBulletPoint(FText Text, EBulletIcon Icon, const FEnchantmentData& Enchantment)
		: FItemBulletPoint(Text, Icon, ItemTag::Unset, false, Enchantment) {
	}


	FItemBulletPoint(FText Text, EBulletIcon Icon, ItemTag TagIcon, bool Upgraded)
		: FItemBulletPoint(Text, Icon, TagIcon, Upgraded, {}) {
	}


	FItemBulletPoint(FText Text, EBulletIcon Icon) 
		: FItemBulletPoint(Text, Icon, {}) {
	}

	FItemBulletPoint()
		: FItemBulletPoint(FText(), EBulletIcon::Generic) {
	}

	static EBulletIcon IconFromRarity(EItemRarity Rarity);

	static FItemBulletPoint CreateTargetsMaxiumumBulletPoint(int targets);
	static FItemBulletPoint CreateSoulGatherBulletPoint(int soulGatherCount);	
	static FItemBulletPoint CreateCooldownBulletPoint(float cooldownSeconds);
	static FItemBulletPoint CreateCharacteristicBulletPoint(const FItemCharacteristic&);
	static FText GetItemPowerRangeText(const game::item::generator::PowerRange&);
	static FItemBulletPoint CreateEstimatedItemPowerRangeBulletPoint(const game::item::generator::PowerRange&);
	static FItemBulletPoint CreateUpgradedItemPowerRangeBulletPoint(const game::item::generator::PowerRange&);
	static FItemBulletPoint CreateSeasonalLimitedBulletPoint();
	static FItemBulletPoint CreateItemPowerRangeBulletPoint(const game::item::generator::PowerRange&);
	static FItemBulletPoint CreateLabeledCounterBulletPoint(const FText& label, int current, int max);
	static FItemBulletPoint CreateLabeledCounterBulletPoint(const FText& label, int current);
	static FText GetAnyAncientChanceText(float anyChance);
	static FText GetAverageAncientsText(float average);
	static FItemBulletPoint CreateAnyAncientChanceBulletPoint(float anyChance);
	static FItemBulletPoint CreateAverageAncientsBulletPoint(float average);
};

