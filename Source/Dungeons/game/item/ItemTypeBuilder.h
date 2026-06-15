#pragma once

#include "ItemType.h"
#include "ItemCharacteristic.h"
#include "game/levels.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "util/LazyEval.h"

namespace game { namespace item { namespace type { 

class ItemTypeBuilder {
public:
	// Note: BlueprintName has to match the folder name in the BP Items folder, if it's inside another folder in the Items folder the folder name also has to be included in blueprintName
	ItemTypeBuilder(const FName& nameId, const FText& name, const FText& description, const FString& blueprintName);
	ItemTypeBuilder(const FName& nameId, const FText& name, const FString& blueprintName); //Some items should no longer have descriptions
	ItemTypeBuilder& consumable(unsigned int consumeAmount = 1);
	ItemTypeBuilder& instant();
	ItemTypeBuilder& gear();
	ItemTypeBuilder& tag(ItemTag);
	ItemTypeBuilder& soulGatherItem(unsigned int gatherSouls = 1);
	ItemTypeBuilder& soulUseItem(float soulCost = 1);
	ItemTypeBuilder& unique();
	ItemTypeBuilder& active();
	ItemTypeBuilder& passive();
	ItemTypeBuilder& rangedWeaponType(RangedWeaponType);
	ItemTypeBuilder& inventoryOnly();
	ItemTypeBuilder& localActivateOnly();
	ItemTypeBuilder& flavour(FText characteristic);
	ItemTypeBuilder& characteristic(FItemCharacteristic);
	ItemTypeBuilder& characteristics(TArray<FItemCharacteristic>);
	ItemTypeBuilder& permanentEnchantments(TArray<FEnchantmentDataWithRarity>);
	ItemTypeBuilder& blockedEnchantments(TArray<FEnchantmentData>);
	ItemTypeBuilder& cooldown(float sec);
	ItemTypeBuilder& duration(float sec);
	ItemTypeBuilder& storeCount(unsigned int count);
	ItemTypeBuilder& workInProgress();
	ItemTypeBuilder& restrictTo(const std::function<TMap<ELevelNames, game::FDifficulty>()>&);
	ItemTypeBuilder& restrictToSameAs(const ItemType&);
	ItemTypeBuilder& difficultyThreshold(const FDifficulty&);
	ItemTypeBuilder& onlyCppInstance(const TSubclassOf<AItemInstance>&);
	ItemTypeBuilder& slotType(ESlotType);
	ItemTypeBuilder& addArmorProperties(const TArray<FArmorPropertyData>&);
	ItemTypeBuilder& removeArmorProperties(const TArray<FArmorPropertyData>&);
	ItemTypeBuilder& generateArmorProperties(unsigned int good);
	ItemTypeBuilder& applyArmorPropertyColor();
	ItemTypeBuilder& matchEnchantmentColorWithArmorPropertyColor();
	ItemTypeBuilder& fixedRarity(EItemRarity);
	ItemTypeBuilder& dropChance(float dropChance);
	ItemTypeBuilder& vendorBlocked();
	ItemTypeBuilder& eventType(EItemEventType eventType);
	ItemTypeBuilder& parent(const ItemType* type);
	ItemTypeBuilder& archetype(EItemArchetype);
	ItemTypeBuilder& archetypes(TArray<EItemArchetype>);

	const ItemType& create() const;

private:
	FName mItemId;
	FText mName;
	FString mBlueprintName;
	FText mDescription;
	FText mFlavour;
	TArray<FItemCharacteristic> mCharacteristics;
	TArray<FEnchantmentDataWithRarity> mPermanentEnchantments;
	TArray<FEnchantmentData> mBlockedEnchantments;
	ItemPersistenceType mPersistenceType = ItemPersistenceType::Permanent;
	ItemUsageType mUsageType = ItemUsageType::Passive;
	RangedWeaponType mRangedWeaponType = RangedWeaponType::None;
	ItemTag mTag = ItemTag::Unset;
	bool mIsUniqueItem = false;
	unsigned int mGatherSouls = 0;
	unsigned int mConsumeAmount = 1;
	float mSoulCost = 0;	
	float mCooldownSeconds = 0;
	float mDurationSeconds = 0;
	unsigned int mStoreCount = 1;
	bool mWorkInProgress = false;
	ItemType::MissionRestriction mRestrictedToMissionDifficulties;
	FDifficulty mDifficultyThreshold = FDifficulty::LOWEST;
	TSubclassOf<AItemInstance> mCppInstance;
	ESlotType mSlotType = ESlotType::Any;
	unsigned int mGenerateArmorProperties = 0;
	TArray<FArmorPropertyData> mArmorProperties;
	TArray<FArmorPropertyData> mBlockedArmorProperties;
	bool mShouldApplyArmorPropertyColor = false;
	bool mShouldMatchEnchantmentColorWithArmorPropertyColor = false;
	TOptional<EItemRarity> mFixedRarity;
	float mDropChance = 1.0f; // D11.DB - Lets us customise drop probabilities via the ItemTypeBuilder.
	bool mVendorBlocked = false; 
	EItemEventType mEventType = EItemEventType::None;
	const ItemType* mParent;
	TArray<EItemArchetype> mArchetypes;
};

}}}
