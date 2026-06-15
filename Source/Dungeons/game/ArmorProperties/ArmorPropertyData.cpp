#include "Dungeons.h"
#include "ArmorPropertyData.h"


FArmorPropertyData::FArmorPropertyData(EArmorPropertyID id, EItemRarity Rarity)
	: ID(id)
	, Rarity(Rarity) {
}

FArmorPropertyData::FArmorPropertyData(EArmorPropertyID id)
	: ID(id)
	, Rarity(EItemRarity::Common) {
}

FArmorPropertyData::FArmorPropertyData()
	: ID(EArmorPropertyID::Unset)
	, Rarity(EItemRarity::Common) {
}

bool FArmorPropertyData::operator==(const FArmorPropertyData& other) const {
	return ID == other.ID
		&& Rarity == other.Rarity;
}

