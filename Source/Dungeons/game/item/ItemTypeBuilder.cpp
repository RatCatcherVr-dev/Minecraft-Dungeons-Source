#include "Dungeons.h"
#include "ItemTypeBuilder.h"
#include "game/difficulty/Difficulty.h"
#include "game/mission/MissionDefs.h"



namespace game { namespace item { namespace type {


ItemTypeBuilder::ItemTypeBuilder(const FName& nameId, const FText& name, const FText& description, const FString& blueprintName)
	: mItemId(nameId)
	, mName(name)
	, mBlueprintName(blueprintName)
	, mDescription(description)
	, mRestrictedToMissionDifficulties(RETLAMBDA(true))
	, mParent(nullptr) {
}

ItemTypeBuilder::ItemTypeBuilder(const FName& nameId, const FText& name, const FString& blueprintName)
	: mItemId(nameId)
	, mName(name)
	, mBlueprintName(blueprintName)
	, mRestrictedToMissionDifficulties(RETLAMBDA(true))
	, mParent(nullptr) {
}

ItemTypeBuilder& ItemTypeBuilder::consumable(unsigned int consumeAmount) {
	mPersistenceType = ItemPersistenceType::Consumable;
	mConsumeAmount = consumeAmount;
	mFixedRarity = EItemRarity::Common;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::instant() {
	mPersistenceType = ItemPersistenceType::Instant;
	mFixedRarity = EItemRarity::Common;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::gear() {
	mPersistenceType = ItemPersistenceType::Gear;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::tag(ItemTag tag) {
	mTag = tag;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::flavour(FText flavour) {
	mFlavour = flavour;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::characteristic(FItemCharacteristic characteristic) {
	mCharacteristics.Add(characteristic);
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::characteristics(TArray<FItemCharacteristic> characteristics) {
	mCharacteristics = characteristics;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::permanentEnchantments(TArray<FEnchantmentDataWithRarity> enchantments) {
	mPermanentEnchantments = enchantments;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::blockedEnchantments(TArray<FEnchantmentData> enchantments) {	
	mBlockedEnchantments = enchantments;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::soulGatherItem(unsigned int souls) {	
	mGatherSouls = souls;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::soulUseItem(float soulCost) {	
	mSoulCost = soulCost;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::unique() {
	mIsUniqueItem = true;
	mFixedRarity = EItemRarity::Unique;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::active() {
	mUsageType = ItemUsageType::Active;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::passive() {
	mUsageType = ItemUsageType::Passive;	
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::rangedWeaponType(RangedWeaponType type) {
	mRangedWeaponType = type;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::inventoryOnly() {
	mUsageType = ItemUsageType::InventoryOnly;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::localActivateOnly() {
	mUsageType = ItemUsageType::LocalActivateOnly;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::cooldown(float seconds) {
	mCooldownSeconds = seconds;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::duration(float seconds) {
	mDurationSeconds = seconds;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::storeCount(unsigned int count) {
	mStoreCount = count;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::workInProgress() {
	mWorkInProgress = true;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::restrictTo(const std::function<TMap<ELevelNames, game::FDifficulty>()>& restrictions) {
	mRestrictedToMissionDifficulties = ItemType::MissionRestriction([restrictions](TMap<ELevelNames, game::FDifficulty>& out) {
		if (missions::areMissionDefsInitialized()) {
			out = restrictions();
			return true;
		}
		return false;
	});
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::restrictToSameAs(const ItemType& other) {
	other.copyMissionRestrictionTo(mRestrictedToMissionDifficulties);
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::difficultyThreshold(const FDifficulty& difficulty) {
	mDifficultyThreshold = difficulty;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::onlyCppInstance(const TSubclassOf<AItemInstance>& ii) {
	mCppInstance = ii;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::slotType(const ESlotType slotType) {
	mSlotType = slotType;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::addArmorProperties(const TArray<FArmorPropertyData>& properties) {
	for (const FArmorPropertyData& ArmorProperty : properties)
	{
		if (!mBlockedArmorProperties.Contains(ArmorProperty))
		{
			mArmorProperties.Add(ArmorProperty);
		}
	}
	
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::removeArmorProperties(const TArray<FArmorPropertyData>& properties) {
	mBlockedArmorProperties.Append(properties);
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::applyArmorPropertyColor() {
	mShouldApplyArmorPropertyColor = true;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::generateArmorProperties(const unsigned int good) {
	mGenerateArmorProperties = good;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::matchEnchantmentColorWithArmorPropertyColor() {
	mShouldMatchEnchantmentColorWithArmorPropertyColor = true;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::fixedRarity(const EItemRarity rarity) {
	mFixedRarity = rarity;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::dropChance(const float dropChance) {
	mDropChance = dropChance;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::vendorBlocked() {
	mVendorBlocked = true;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::eventType(EItemEventType eventType) {
	mEventType = eventType;
	return *this;
}

ItemTypeBuilder & ItemTypeBuilder::parent(const ItemType* type) {
	mParent = type;
	return *this;
}

ItemTypeBuilder& ItemTypeBuilder::archetype(EItemArchetype archetype) {
	return archetypes({ archetype });
}

ItemTypeBuilder& ItemTypeBuilder::archetypes(TArray<EItemArchetype> archetypes) {
	ensureMsgf(mArchetypes.Num() == 0, TEXT("Archetypes already added!"));
	mArchetypes = std::move(archetypes);
	return *this;
}

const ItemType& ItemTypeBuilder::create() const {
	auto id = GetItemRegistry().Register(mItemId, MakeUnique<const ItemType>(
		mItemId,
		mName,
		mBlueprintName,
		mDescription, 
		mFlavour, 
		mCharacteristics, 
		mPersistenceType,
		mUsageType,
		mRangedWeaponType,
		mTag,
		mGatherSouls,
		mIsUniqueItem,
		mSoulCost,
		mCooldownSeconds,
		mDurationSeconds,
		mStoreCount,
		mConsumeAmount,
		mWorkInProgress,
		mRestrictedToMissionDifficulties,
		mDifficultyThreshold,
		mCppInstance,
		mSlotType, 
		mArmorProperties,
		mGenerateArmorProperties,
		mPermanentEnchantments,
		mBlockedEnchantments,
		mShouldApplyArmorPropertyColor,
		mShouldMatchEnchantmentColorWithArmorPropertyColor,
		mFixedRarity,
		mDropChance,
		mVendorBlocked,
		mEventType,
		mParent,
		mArchetypes
		));

	return GetItemRegistry().Get(id);
}}}}
