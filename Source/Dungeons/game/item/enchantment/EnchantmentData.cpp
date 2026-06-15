#include "Dungeons.h"
#include "EnchantmentData.h"

bool FEnchantmentData::operator==(const FEnchantmentData& other) const {
	return TypeID == other.TypeID
		&& Level == other.Level
		&& Category == other.Category
		&& Source == other.Source;
}

FEnchantmentData::FEnchantmentData(EEnchantmentTypeID type, int level /*= 0*/, EEnchantmentCategory category /*= EEnchantmentCategory::Unset*/, EEnchantmentSource source /*= EEnchantmentSource::Unset*/) 
	: TypeID(type)
	, Level(level)
	, Category(category)
	, Source(source) 
{}

FEnchantmentData::FEnchantmentData() 
	: TypeID(EEnchantmentTypeID::Unset)
	, Level(0)
	, Category(EEnchantmentCategory::Unset)
	, Source(EEnchantmentSource::Unset) 
{}

FEnchantmentDataWithRarity::FEnchantmentDataWithRarity(FEnchantmentData enchantment, EItemRarity rarity /*= EItemRarity::Common*/) 
	: Enchantment(enchantment)
	, Rarity(rarity) 
{}

FEnchantmentDataWithRarity::FEnchantmentDataWithRarity() 
	: Rarity(EItemRarity::Common) 
{}
