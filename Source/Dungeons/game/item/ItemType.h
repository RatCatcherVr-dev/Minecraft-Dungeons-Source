#pragma once

#include "ItemCharacteristic.h"
#include "ItemTypeDefs.h"
#include "game/levels.h"
#include "game/UniqueId.h"
#include "game/component/EnchantmentComponent.h"
#include "game/difficulty/Difficulty.h"
#include "util/LazyEval.h"
#include "game/ArmorProperties/ArmorPropertyEnumTypes.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/dlc/DLCName.h"
#include "game/item/ItemArchetype.h"
#include <SubclassOf.h>

enum class ItemTag : uint8;
enum class ItemTagLevel : uint8;
enum class ESlotType : uint8;

class AItemInstance;
class AStorableItem;
class AItem;

template class DUNGEONS_API TIdRegistry<FName, const ItemType>;
struct FEnchantmentData;
using FItemRegistry = TIdRegistry<FName, const ItemType>;
using FItemId = FItemRegistry::TId;

DUNGEONS_API  FItemRegistry& GetItemRegistry();

enum class ItemPersistenceType {
	Consumable,
	Permanent, 
	Gear,
	Instant
};

enum class ItemUsageType {
	Active,
	Passive,
	InventoryOnly,
	LocalActivateOnly,
};

UENUM(BlueprintType)
enum class RangedWeaponType : uint8 {
	None,
	Bow,
	Crossbow
};

UENUM(BlueprintType)
enum class EItemEventType : uint8 {
	None,
	Spooky,
	Winter,
	Year1,
	Spooky2
};
ENUM_NAME(EItemEventType);

class DUNGEONS_API ItemType {
public:
	using MissionRestriction = lazyeval::OutParameterRefreshable0<TMap<ELevelNames, game::FDifficulty>>;

	ItemType(const FName& itemId,
			const FText& name,
			const FString& blueprintClass,
			const FText& description,
			const FText& flavour,
			const TArray<FItemCharacteristic> characteristics,
			ItemPersistenceType,
			ItemUsageType,
			RangedWeaponType,
			ItemTag,
			unsigned int gatherSouls,
			bool isUniqueItem,
			float soulCost,
			float cooldown,
			float duration,
			unsigned int storeCount,
			unsigned int mConsumeAmount,
			bool workInProgress,
			const MissionRestriction& restrictedToMissionDifficulties,
			game::FDifficulty difficultyThreshold,
			const TSubclassOf<AItemInstance>& instance,
			ESlotType slotType, 
			TArray<FArmorPropertyData> armorProperties,
			unsigned int generateArmorProperties,
			TArray<FEnchantmentDataWithRarity> enchantments,
			TArray<FEnchantmentData> blockedEnchantments,
			bool shouldApplyArmorPropertyColor,
			bool shouldMatchEnchantmentColorWithArmorPropertyColor,
			TOptional<EItemRarity> rarity,
			float dropChance,
			bool vendorBlocked,
			EItemEventType eventType,
			const ItemType* parent,
			TArray<EItemArchetype> archetypes);

	ItemType(const ItemType&) = delete;
	ItemType(ItemType&&) = default;

	const ItemType& operator=(const ItemType&) = delete;
	ItemType& operator=(ItemType&&) = default;

	bool operator==(const ItemType&) const;
	bool operator!=(const ItemType&) const;

	FName getNameId() const;
	FItemId getId() const;
	ItemTag getTag() const;


	const FString& getName() const;
	const FString& getDescription() const;
	const FText& getNameText() const;
	const FText& getFlavourText() const;
	const FText& getDescriptionText() const;
	const TArray<FItemCharacteristic>& getCharacteristics() const;
	TArray<FItemCharacteristic> getEnchantmentCharacteristics() const;
	TSubclassOf<AStorableItem> getStorableClass() const;
	TSubclassOf<AItemInstance> getInstanceClass(bool load = true) const;
	void					   pinStorableClass() const;

	FSoftObjectPath getInstancePath() const;
	
	const FName& getRelativeIconPathHigh() const { return mRelativeIconPathHigh; }
	const FName& getRelativeIconPath() const { return mRelativeIconPath; }
	const FName& getRelativeGearIconPath() const { return mRelativeGearIconPath; }
	const FName& getRelativeAmmoIconSmallPath() const { return mRelativeAmmoIconSmallPath; }
	const FName& getRelativeStorableObjectPath() const { return mRelativeStorableObjectPath; }
	const FName& getRelativeInstanceObjectPath() const { return mRelativeInstanceObjectPath; }
	
	RangedWeaponType getRangedWeaponType() const;

	int getSoulGatherCount() const;
	float getSoulCost() const;	
	bool hasCooldown() const;
	float getCoolDownSeconds() const;
	float getDurationSeconds() const;
	unsigned int getStoreCount() const;
	unsigned int getConsumeAmount() const;
	unsigned int getNumGeneratedArmorProperties() const;
	TArray<EArmorPropertyID> getDefaultArmorPropertyIds() const;
	TArray<FArmorPropertyData> getDefaultArmorProperties(const TSet<ELevelNames>& unlockedLevels) const;
	TArray<FEnchantmentData> getDefaultEnchantments() const;
	TArray<FEnchantmentData> getBlockedEnchantments() const;
	bool shouldApplyArmorPropertyColor() const;
	bool enchantmentColorMatchesArmorPropertyColor() const;
	bool shouldShowLockedOwnerOutline() const;

	TOptional<EItemRarity> getFixedRarity() const;
	float getDropChance() const { return mDropChance; }
	bool isVendorBlocked() const { return mVendorBlocked; }
	
	// Persistence
	bool isPermanent() const;
	bool isConsumable() const;
	bool isInstant() const;
	bool isGear() const;
	bool isToken() const;

	// Usage
	bool isActive() const;
	bool isPassive() const;
	bool isInventoryOnly() const;
	bool isLocalActivateOnly() const;

	// Uses
	bool usesItemPower() const;
	bool usesRarity() const;

	bool isUpgradable() const;

	// Ranged weapon type
	bool isBow() const;	
	bool isCrossbow() const;

	ESlotType getSlotType() const;

	bool hasTag(ItemTag tag) const;
	bool isSoulGatherBoostingItem() const;
	bool isSoulGatherItem() const;
	bool isSoulUseItem() const;
	bool isUnique() const;
	bool isWorkInProgress() const;
	bool isAllowedOnMissionDifficulty(ELevelNames mission, const game::FDifficulty& difficulty) const;
	const game::FDifficulty& difficultyThreshold() const;
	const TMap<ELevelNames, game::FDifficulty>& getRestrictedMissionDifficulties() const;
	bool isAllowedOnDifficulty(const game::FDifficulty&) const;
	TOptional<EDLCName> getDLCEligibility() const;

	void copyMissionRestrictionTo(MissionRestriction&) const;

	bool accepts(ESlotType slotType) const;
	ESlotType slotType() const;

	bool isEventItem() const;
	EItemEventType getEventType() const;

	const TArray<EItemArchetype>& getArchetypes() const;

	STAT(TStatId tickItemStatId;)
		STAT(TStatId activateInstanceStatId;)

	const ItemType* getParent() const;
	const ItemType* getRoot() const;

	static void PreloadAllItemClassTypes();
private:
	mutable TOptional<FItemId> mItemId;
	FName mNameId;
	FText mName;
	FString mBlueprintName;
	FText mDescription;
	FText mFlavour;
	TArray<FItemCharacteristic> mCharacteristics;
	ItemPersistenceType mPersistenceType;
	ItemUsageType mUsageType;
	RangedWeaponType mRangedWeaponType;
	ItemTag mTag;
	bool mIsSoulGatherItem;
	bool mIsSoulUseItem;
	bool mIsUniqueItem;
	bool mIsCharging;
	int mGatherSouls;
	float mSoulCost;	
	float mCooldownSeconds;
	float mDurationSeconds;
	unsigned mStoreCount;
	unsigned mConsumeAmount;
	bool mWorkInProgress;
	MissionRestriction mRestrictedToMissionDifficulties;
	game::FDifficulty mDifficultyThreshold;
	float mDifficultyThresholdFraction;
	mutable TWeakObjectPtr<UClass> mStorableClass;
	mutable TWeakObjectPtr<UClass> mInstanceClass;
	ESlotType mSlotType;
	unsigned int mGenerateArmorProperties;
	TArray<FArmorPropertyData> mDefaultArmorProperties;
	TArray<FEnchantmentDataWithRarity> mDefaultEnchantments;
	TArray<FEnchantmentData> mBlockedEnchantments;
	bool mShouldApplyArmorPropertyColor;
	bool mShouldMatchEnchantmentColorWithArmorPropertyColor;
	TOptional<EItemRarity> mFixedRarity;
	float mDropChance;
	bool mVendorBlocked;
	EItemEventType mEventType;
	TArray<EItemArchetype> mArchetypes;

	FName mRelativeIconPathHigh;
	FName mRelativeIconPath;
	FName mRelativeGearIconPath;
	FName mRelativeAmmoIconSmallPath;
	FName mRelativeStorableObjectPath;
	FName mRelativeInstanceObjectPath;

	const ItemType* mParent;
};
