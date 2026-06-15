#include "Dungeons.h"
#include "ItemType.h"
#include "game/difficulty/Difficulty.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/ArmorProperties/ArmorPropertiesUtil.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/StreamableManager.h"
#include "util/ClassUtil.h"
#include "util/Algo.hpp"
#include "game/actor/item/StorableItem.h"
#include "game/item/instance/AItemInstance.h"
#include "Assets/ItemAssetFinder.h"
#include "AbilitySystemGlobals.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "util/DefsUtil.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/enchantment/EnchantmentData.h"

#if WITH_EDITOR
#include "util/EnumUtil.h"
#endif


DECLARE_STATS_GROUP(TEXT("Items"), STATGROUP_Items, STATCAT_Advanced);

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

////////////////////////////////////////////////////////////////
// ItemType
////////////////////////////////////////////////////////////////
ItemType::ItemType(const FName& itemId,
                   const FText& name,
                   const FString& blueprintName,
				   const FText& description,
				   const FText& flavour,
				   const TArray<FItemCharacteristic> characteristics,

				   ItemPersistenceType persistenceType,
				   ItemUsageType usageType,
				   RangedWeaponType rangedWeaponType,
				   ItemTag tag,				   
				   unsigned int gatherSouls,				   
				   bool isUniqueItem,
				   float soulCost,   
				   float cooldown,
				   float duration,
				   unsigned int storeCount,
				   unsigned int consumeAmount,
				   bool workInProgress, 
				   const MissionRestriction& restrictedToMissionDifficulties,
				   game::FDifficulty difficultyThreshold,
				   const TSubclassOf<AItemInstance>& cppInstanceClass,
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
				   TArray<EItemArchetype> archetypes) :
	mNameId(itemId),
	mName(name),
	mBlueprintName(blueprintName),
	mDescription(description),
	mFlavour(flavour),
	mCharacteristics(characteristics),
	mPersistenceType(persistenceType),
	mUsageType(usageType),
	mRangedWeaponType(rangedWeaponType),
	mTag(tag),
	mGatherSouls(gatherSouls),
	mIsUniqueItem(isUniqueItem),
	mSoulCost(soulCost),	
	mCooldownSeconds(cooldown),
	mDurationSeconds(duration),
	mStoreCount(storeCount),
	mConsumeAmount(consumeAmount),
	mWorkInProgress(workInProgress),
	mRestrictedToMissionDifficulties(restrictedToMissionDifficulties),
	mDifficultyThreshold(difficultyThreshold),
	mDifficultyThresholdFraction(difficultyThreshold.combinedFraction()),
	mStorableClass(nullptr),
	mSlotType(slotType), 
	mGenerateArmorProperties(generateArmorProperties),
	mDefaultArmorProperties(armorProperties),
	mDefaultEnchantments(enchantments),
	mBlockedEnchantments(blockedEnchantments),
	mShouldApplyArmorPropertyColor(shouldApplyArmorPropertyColor),
	mShouldMatchEnchantmentColorWithArmorPropertyColor(shouldMatchEnchantmentColorWithArmorPropertyColor),
	mFixedRarity(rarity),
	mDropChance(dropChance),
	mVendorBlocked(vendorBlocked),
	mEventType(eventType),
	mRelativeIconPathHigh(game::defs::generateTextureRelativePath(mBlueprintName, "_Icon")),
	mRelativeIconPath(game::defs::generateTextureRelativePath(mBlueprintName, "_Icon_Inventory")),
	mRelativeGearIconPath(game::defs::generateTextureRelativePath(mBlueprintName, "_GearIcon")),
	mRelativeAmmoIconSmallPath(game::defs::generateTextureRelativePath(mBlueprintName, "_AmmoIconSmall")),
	mRelativeStorableObjectPath(game::defs::generateBlueprintRelativePath(mBlueprintName, "Storable")),
	mRelativeInstanceObjectPath(game::defs::generateBlueprintRelativePath(mBlueprintName, "Instance")),
	mParent(parent),
	mArchetypes(std::move(archetypes))
{
	if (!workInProgress && (isGear() || isPermanent())) {
		ensureMsgf(mArchetypes.Num() != 0, TEXT("No archetypes added for this itemtype"));
	}
	if (cppInstanceClass) {
		mInstanceClass = cppInstanceClass.Get();
	}

	STAT(tickItemStatId = FDynamicStats::CreateStatId<FStatGroup_STATGROUP_Items>(getName() + "_TickItem");)
	STAT(activateInstanceStatId = FDynamicStats::CreateStatId<FStatGroup_STATGROUP_Items>(getName() + "_ActivateInstance");)
}

template <typename T>
void debugVerifyCorrectBlueprintClass(UClass* cls, const FItemId& type, const FString& name) {
#if WITH_EDITOR
	if (!cls) {
		UE_LOG(LogDungeons, Warning, TEXT("%s class not found: %s."), *name, *type.GetBackingType().ToString());
		return;
	}
	const FItemId foundType = Cast<T>(cls->GetDefaultObject())->GetItemType().getId();
	if (foundType != type) {
		UE_LOG(LogDungeons, Warning, TEXT("%s class does not have correct ItemType set. Found %s, expected %s."), *name, *foundType.GetBackingType().ToString(), *type.GetBackingType().ToString());
	}
#endif
}

FItemRegistry& GetItemRegistry()
{
	static FItemRegistry instance;
	return instance;
}

const ItemType * ItemType::getParent() const
{
	return mParent;
}

const ItemType * ItemType::getRoot() const {
	const ItemType *root = this;
	const ItemType *currParent = root->getParent();
	while (currParent) {
		root = currParent;
		currParent = root->getParent();
	}

	return root;
}

// Deprecated: should use localizable FText- object types
const FString& ItemType::getName() const {
	return mName.ToString();
}

// Deprecated: should use localizable FText- object apes
const FString& ItemType::getDescription() const {
	return mDescription.ToString();
}

const FText& ItemType::getNameText() const {
	return mName;
}

const FText& ItemType::getDescriptionText() const {
	return mDescription;
}

const FText& ItemType::getFlavourText() const {
	return mFlavour;
}

const TArray<FItemCharacteristic>& ItemType::getCharacteristics() const {
	return mCharacteristics;
}

TArray<FItemCharacteristic> ItemType::getEnchantmentCharacteristics() const {
	TArray<FItemCharacteristic> enchantmentBulletPoints;
	for (auto enchantment : mDefaultEnchantments) {
		const auto& type = game::enchantment::type::getEnchantmentType(enchantment.Enchantment.TypeID);

		if (!type.isHiddenInInspector()) {
			enchantmentBulletPoints.Emplace(type.getCharacteristicText(), enchantment.Rarity, enchantment.Enchantment);
		}
	}
	return enchantmentBulletPoints;
}

TSubclassOf<AStorableItem> ItemType::getStorableClass() const {
	
	if (!mStorableClass.IsValid()) {
		mStorableClass = StaticLoadClass(AStorableItem::StaticClass(), nullptr, *IDungeonsModule::Get().GetItemAssetFinder()->StorablePath(getId()).Get(FSoftObjectPath()).ToString());
		debugVerifyCorrectBlueprintClass<AStorableItem>(mStorableClass.Get(), getId(), "Storable");
	}
	return mStorableClass.Get();
	
}

TSubclassOf<AItemInstance> ItemType::getInstanceClass(bool load) const {
	if (!mInstanceClass.IsValid() && load) {
		mInstanceClass = StaticLoadClass(AItemInstance::StaticClass(), nullptr, *IDungeonsModule::Get().GetItemAssetFinder()->InstancePath(getId()).Get(FSoftObjectPath()).ToString());
		debugVerifyCorrectBlueprintClass<AItemInstance>(mInstanceClass.Get(), getId(), "Instance");
	}
	return mInstanceClass.Get();
}

FSoftObjectPath ItemType::getInstancePath() const {
	return IDungeonsModule::Get().GetItemAssetFinder()->InstancePath(getId()).Get(FSoftObjectPath());
}

bool ItemType::operator==(const ItemType& rhs) const {
	return mNameId == rhs.mNameId;
}

bool ItemType::operator!=(const ItemType& rhs) const {
	return !(*this == rhs);
}

FName ItemType::getNameId() const {
	return mNameId;
}

FItemId ItemType::getId() const {
	if(!mItemId) {
		mItemId = GetItemRegistry().Request(mNameId);
	}
	
	return mItemId.GetValue();
}

ItemTag ItemType::getTag() const {
	return mTag;
}

float ItemType::getCoolDownSeconds() const {
	return mCooldownSeconds;
}

float ItemType::getDurationSeconds() const {
	return mDurationSeconds;
}

unsigned ItemType::getStoreCount() const {
	return mStoreCount;
}

unsigned ItemType::getConsumeAmount() const {
	return mConsumeAmount;
}

unsigned int ItemType::getNumGeneratedArmorProperties() const
{
	return mGenerateArmorProperties;
}

TArray<EArmorPropertyID> ItemType::getDefaultArmorPropertyIds() const {
	TArray<EArmorPropertyID> ids;
	for (const FArmorPropertyData& propdata : mDefaultArmorProperties) {
		ids.Add(propdata.ID);
	}
	return ids;
}

TArray<FArmorPropertyData> ItemType::getDefaultArmorProperties(const TSet<ELevelNames>& unlockedLevels) const {
	auto Props = mDefaultArmorProperties;

	const auto numGenerated = getNumGeneratedArmorProperties();
	if (numGenerated > 0) {
		auto generatedProps = game::armorproperties::generateRandomArmorProperties(numGenerated, unlockedLevels);
		Props.Append(generatedProps);
	}
	return Props;
}

TArray<FEnchantmentData> ItemType::getDefaultEnchantments() const {
	return algo::map_tarray(mDefaultEnchantments, RETLAMBDA(it.Enchantment));
}

TArray<FEnchantmentData> ItemType::getBlockedEnchantments() const {
	return mBlockedEnchantments;
}

bool ItemType::shouldApplyArmorPropertyColor() const {
	return hasTag(ItemTag::Armor) ? mShouldApplyArmorPropertyColor : false;
}

bool ItemType::enchantmentColorMatchesArmorPropertyColor() const {
	return hasTag(ItemTag::Armor) ? mShouldMatchEnchantmentColorWithArmorPropertyColor : false;
}

//D11.KS - Currency should be the only items that are soulbound that do not have a constant outline.
bool ItemType::shouldShowLockedOwnerOutline() const {
	return !hasTag(ItemTag::Currency);
}

TOptional<EItemRarity> ItemType::getFixedRarity() const {
	return mFixedRarity;
}

bool ItemType::hasCooldown() const {
	return mCooldownSeconds > 0;
}

int ItemType::getSoulGatherCount() const {
	return mGatherSouls;
}

float ItemType::getSoulCost() const {
	return mSoulCost;
}



bool ItemType::isActive() const {
	return mUsageType == ItemUsageType::Active;
}

bool ItemType::isPassive() const {
	return mUsageType == ItemUsageType::Passive;
}

bool ItemType::isPermanent() const {
	return mPersistenceType == ItemPersistenceType::Permanent;
}

bool ItemType::isConsumable() const {
	return mPersistenceType == ItemPersistenceType::Consumable;
}

bool ItemType::isInstant() const {
	return mPersistenceType == ItemPersistenceType::Instant;
}

bool ItemType::isGear() const {
	return mPersistenceType == ItemPersistenceType::Gear;
}

bool ItemType::isToken() const {
	return hasTag(ItemTag::Token);
}

bool ItemType::hasTag(ItemTag tag) const {
	return mTag == tag;
}

bool ItemType::isSoulGatherBoostingItem() const {
	//Randomized armor properties are not evaluated here since they only exist outside the type. (The properties mystery armor generates will thus never be checked)
	return (algo::any_of(mDefaultArmorProperties, RETLAMBDA(game::armorproperties::type::getArmorPropertyType(it.ID).hasTag(EArmorPropertyTag::SoulGatheringBoost))));
}

bool ItemType::isSoulGatherItem() const {
	return mGatherSouls > 0;
}

bool ItemType::isSoulUseItem() const {
	return mSoulCost > 0;
}

bool ItemType::isUnique() const {
	return mIsUniqueItem;
}

bool ItemType::isInventoryOnly() const {
	return mUsageType == ItemUsageType::InventoryOnly;
}

bool ItemType::isLocalActivateOnly() const {
	return mUsageType == ItemUsageType::LocalActivateOnly;
}

bool ItemType::usesItemPower() const {
	return isGear() || isPermanent();
}

bool ItemType::usesRarity() const {
	return isGear() || isPermanent();
}

bool ItemType::isUpgradable() const {
	return (isPermanent() || isGear()) && !(getId() == game::item::type::DiamondDust.getId());
}

bool ItemType::isBow() const {
	return hasTag(ItemTag::RangedWeapon) ? (mRangedWeaponType == RangedWeaponType::Bow) : false;
}

bool ItemType::isCrossbow() const {
	return hasTag(ItemTag::RangedWeapon) ? (mRangedWeaponType == RangedWeaponType::Crossbow) : false;
}

ESlotType ItemType::getSlotType() const {
	return mSlotType;
}

RangedWeaponType ItemType::getRangedWeaponType() const {
	return mRangedWeaponType;
}

bool ItemType::isWorkInProgress() const {
	return mWorkInProgress;
}

bool ItemType::isAllowedOnMissionDifficulty(ELevelNames mission, const game::FDifficulty& difficulty) const {
	if (mRestrictedToMissionDifficulties->Num() == 0) {
		return true;
	}

	const game::FDifficulty* foundRestriction = mRestrictedToMissionDifficulties->Find(mission);
	return foundRestriction
		&& difficulty.chosen() >= foundRestriction->chosen()
		&& difficulty.missionDifficultyFraction() >= foundRestriction->missionDifficultyFraction()
		&& difficulty.extraChallengeFraction() >= foundRestriction->extraChallengeFraction();
}

const game::FDifficulty& ItemType::difficultyThreshold() const {
	return mDifficultyThreshold;
}

const TMap<ELevelNames, game::FDifficulty>& ItemType::getRestrictedMissionDifficulties() const {
	return *mRestrictedToMissionDifficulties;
}

TOptional<EDLCName> ItemType::getDLCEligibility() const {
	std::set<EDLCName> itemDLCs;
	for (const auto& mission : getRestrictedMissionDifficulties()) {
		if (const auto m = missions::getChecked(mission.Key)) {
			if (const auto dlc = m->getRequiredDLC()) {
				itemDLCs.insert(dlc.GetValue());
			} else {
				return {};
			}
		}
	}
	return itemDLCs.size() == 1 ? *itemDLCs.begin() : TOptional<EDLCName>();
}

bool ItemType::isAllowedOnDifficulty(const game::FDifficulty& difficulty) const {
	if (difficultyquery::isUnlockedByDefault(difficulty.chosen()) && difficultyquery::isUnlockedByDefault(mDifficultyThreshold.chosen())) {
		// All default-unlocked {mission,difficulty}-combinations should only be compared by their "mission order"
		return difficulty.missionDifficultyFraction() >= mDifficultyThreshold.missionDifficultyFraction();
	}
	return difficulty.combinedFraction() >= mDifficultyThresholdFraction;
}

void ItemType::copyMissionRestrictionTo(MissionRestriction& dst) const {
	dst = mRestrictedToMissionDifficulties;
}

bool ItemType::accepts(ESlotType slotType) const {
	return mSlotType == slotType;
}

ESlotType ItemType::slotType() const {
	return mSlotType;
}

bool ItemType::isEventItem() const {
	return mEventType != EItemEventType::None;
}

EItemEventType ItemType::getEventType() const {
	return mEventType;
}

const TArray<EItemArchetype>& ItemType::getArchetypes() const {
	return mArchetypes;
}

void ItemType::PreloadAllItemClassTypes()
{
	
	//parse preload table in here to determine resident classes
	UDataTable* pDataTable = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/DataTables/Assets/PreloadedStorableItems.PreloadedStorableItems'"), NULL, LOAD_None, NULL);;

	TArray< FSoftObjectPath > Paths;

	if (pDataTable)
	{
		//read soft paths from data table
		pDataTable->ForeachRow<FStorableItemRow>(TEXT("FStorableItemRow"), [&](const FName& Key, const FStorableItemRow& Value) {
			Paths.Push(Value.StorableItemClass.ToSoftObjectPath());			
		});

	}
	
	UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths, [Paths]() {
		
		for (const auto& name : Paths) {
			if (UObject* object = name.ResolveObject()) {
				object->AddToRoot();
				UClass* pClass = Cast<UClass>(object);

				//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("ASSET RESOLVED FOR %s : %s %s %d\n"), *name.ToString(), *object->GetClass()->GetName(), *pClass->GetName(), (int)());
				
				//PreCache any storables so we dont have a search at game time
				if (pClass->IsChildOf<AStorableItem>())
				{
					if(AStorableItem* pDefaultObject = pClass->GetDefaultObject<AStorableItem>()) {
						pDefaultObject->GetItemType().getStorableClass();
					}
				}

			}
			else
			{
				FPlatformMisc::LowLevelOutputDebugStringf(TEXT("NO ASSET FOR %s\n"), *name.ToString());
			}
		}
	});

}
