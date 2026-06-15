#include "Dungeons.h"
#include "EnchantmentType.h"

EnchantmentType::EnchantmentType() {}

EnchantmentType::EnchantmentType(EEnchantmentTypeID type)
	: TypeID(type) {
		GetEffectTemplate = [this](int lv){return this->LevelEffectTemplate;};
}

bool EnchantmentType::hasTag(EEnchantmentTag inTag) const {
	return Tags.Contains(inTag);
}

bool EnchantmentType::hasAnyTags() const {
	return Tags.Num() > 0;
}

bool EnchantmentType::isRarity(EEnchantmentRarity rarity) const {
	return Rarity == rarity;
}

const FText& EnchantmentType::getDisplayName() const {
	return DisplayName;
}

const FText& EnchantmentType::getDescription() const {
	return Description;
}

const FText& EnchantmentType::getCharacteristicText() const {
	return CharacteristicText;
}

bool EnchantmentType::isDisabledForPlayers() const {
	return bIsDisabledForPlayer;
}

bool EnchantmentType::canBeUsedByMobs() const {
	return bCanBeUsedByMobs;
}

int EnchantmentType::getSoulGatherCount() const {
	return mSoulGatherAmount;
}

const EEnchantmentTypeID EnchantmentType::getEnchantmentTypeID() const {
	return TypeID;
}

const EEnchantmentClassification EnchantmentType::getEnchantmentClassification() const {
	return Classification;
}

const EEnchantmentCategory EnchantmentType::getEnchantmentCategory() const {
	return Category;
}

const EEnchantmentRarity EnchantmentType::getRarity() const{
	return Rarity;
}

bool EnchantmentType::isAlwaysEnchantable() const {
	return bIsAlwaysEnchantable;
}

game::FDifficulty EnchantmentType::difficultyThreshold() const {
	return DifficultyThreshold;
}

const FText EnchantmentType::getLevelEffectTemplate(int level) const {
	return GetEffectTemplate(level); 
}

bool EnchantmentType::isWorkInProgress() const {
	return bWorkInProgress;
}

bool EnchantmentType::hasEnchantersTomeFlags(EEnchantersTomeFlags flags) const {
	return EnumHasAnyFlags(EnchantersTomeFlags, flags);
}