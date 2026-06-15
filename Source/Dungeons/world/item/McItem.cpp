/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"
#include "McItem.h"
#include "world/item/ItemCategory.h"
#include "world/level/block/Block.h"
#include "locale/I18n.h"
#include "util/StringUtils.h"
#include "world/item/ItemInstance.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "util/StringByteOutput.h"
#include "util/StringByteInput.h"
#include "util/DataIO.h"
#include "util/JSONUtils.h"

const std::string McItem::ICON_DESCRIPTION_PREFIX("item.");
Random McItem::mRandom;

std::shared_ptr<TextureAtlas> McItem::mItemTextureAtlas;
std::unordered_map<std::string, Unique<McItem>> McItem::mItemLookupMap;
McItem* McItem::mItems[MAX_ITEMS] = { nullptr };

McItem::Tier const McItem::Tier::WOOD(0, 59, 2, 0, 15);
McItem::Tier const McItem::Tier::STONE(1, 131, 4, 1, 5);
McItem::Tier const McItem::Tier::IRON(2, 250, 6, 2, 14);
McItem::Tier const McItem::Tier::DIAMOND(3, 1561, 8, 3, 10);
McItem::Tier const McItem::Tier::GOLD(0, 32, 12, 0, 22);

std::vector<ItemInstance> McItem::mCreativeList;

ItemInstance McItem::Tier::getTierItem(void) const {
	switch (getLevel()) {
	case 0:
		if (getSpeed() == 2) {
			return ItemInstance(Block::mWoodPlanks);
		}
		else {
			return ItemInstance(McItem::mGoldIngot);
		}

	case 1:
		return ItemInstance(Block::mStone);
	case 2:
		return ItemInstance(McItem::mIronIngot);
	case 3:
		return ItemInstance(McItem::mDiamond);
	default:
		return ItemInstance();
	}
}

// Static McItem Definitions
McItem* McItem::mShovel_iron = nullptr;
McItem* McItem::mPickAxe_iron = nullptr;
McItem* McItem::mHatchet_iron = nullptr;
McItem* McItem::mFlintAndSteel = nullptr;
McItem* McItem::mApple = nullptr;
McItem* McItem::mBow = nullptr;
McItem* McItem::mArrow = nullptr;
McItem* McItem::mCoal = nullptr;
McItem* McItem::mDiamond = nullptr;
McItem* McItem::mIronIngot = nullptr;
McItem* McItem::mGoldIngot = nullptr;
McItem* McItem::mSword_iron = nullptr;

McItem* McItem::mSword_wood = nullptr;
McItem* McItem::mShovel_wood = nullptr;
McItem* McItem::mPickAxe_wood = nullptr;
McItem* McItem::mHatchet_wood = nullptr;

McItem* McItem::mSword_stone = nullptr;
McItem* McItem::mShovel_stone = nullptr;
McItem* McItem::mPickAxe_stone = nullptr;
McItem* McItem::mHatchet_stone = nullptr;

McItem* McItem::mSword_diamond = nullptr;
McItem* McItem::mShovel_diamond = nullptr;
McItem* McItem::mPickAxe_diamond = nullptr;
McItem* McItem::mHatchet_diamond = nullptr;

McItem* McItem::mStick = nullptr;
McItem* McItem::mBowl = nullptr;
McItem* McItem::mMushroomStew = nullptr;

McItem* McItem::mSword_gold = nullptr;
McItem* McItem::mShovel_gold = nullptr;
McItem* McItem::mPickAxe_gold = nullptr;
McItem* McItem::mHatchet_gold = nullptr;

McItem* McItem::mString = nullptr;
McItem* McItem::mFeather = nullptr;
McItem* McItem::mSulphur = nullptr;

McItem* McItem::mHoe_wood = nullptr;
McItem* McItem::mHoe_stone = nullptr;
McItem* McItem::mHoe_iron = nullptr;
McItem* McItem::mHoe_diamond = nullptr;
McItem* McItem::mHoe_gold = nullptr;

McItem* McItem::mSeeds_wheat = nullptr;
McItem* McItem::mWheat = nullptr;
McItem* McItem::mBread = nullptr;

McItem* McItem::mHelmet_cloth = nullptr;
McItem* McItem::mChestplate_cloth = nullptr;
McItem* McItem::mLeggings_cloth = nullptr;
McItem* McItem::mBoots_cloth = nullptr;

McItem* McItem::mElytra = nullptr;

McItem* McItem::mHelmet_chain = nullptr;
McItem* McItem::mChestplate_chain = nullptr;
McItem* McItem::mLeggings_chain = nullptr;
McItem* McItem::mBoots_chain = nullptr;

McItem* McItem::mHelmet_iron = nullptr;
McItem* McItem::mChestplate_iron = nullptr;
McItem* McItem::mLeggings_iron = nullptr;
McItem* McItem::mBoots_iron = nullptr;

McItem* McItem::mHelmet_diamond = nullptr;
McItem* McItem::mChestplate_diamond = nullptr;
McItem* McItem::mLeggings_diamond = nullptr;
McItem* McItem::mBoots_diamond = nullptr;

McItem* McItem::mHelmet_gold = nullptr;
McItem* McItem::mChestplate_gold = nullptr;
McItem* McItem::mLeggings_gold = nullptr;
McItem* McItem::mBoots_gold = nullptr;

McItem* McItem::mFlint = nullptr;
McItem* McItem::mPorkChop_raw = nullptr;
McItem* McItem::mPorkChop_cooked = nullptr;
McItem* McItem::mPainting = nullptr;

McItem* McItem::mChorusFruit;
McItem* McItem::mChorusFruitPopped;

McItem* McItem::mApple_gold = nullptr;
McItem* McItem::mApple_enchanted = nullptr;

McItem* McItem::mSign = nullptr;
McItem* McItem::mDoor_wood = nullptr;

McItem* McItem::mBucket = nullptr;
//McItem* McItem::bucket_empty = nullptr;
//McItem* McItem::bucket_water = nullptr;
//McItem* McItem::bucket_lava = nullptr;

McItem* McItem::mMinecart = nullptr;
McItem* McItem::mSaddle = nullptr;
McItem* McItem::mDoor_iron = nullptr;
McItem* McItem::mRedStone = nullptr;
McItem* McItem::mSnowBall = nullptr;

McItem* McItem::mBoat = nullptr;

McItem* McItem::mMobPlacer = nullptr;
McItem* McItem::mExperiencePotionItem = nullptr;
McItem* McItem::mFireCharge = nullptr;

McItem* McItem::mPumpkinPie = nullptr;

McItem* McItem::mLeatherHorseArmor = nullptr;
McItem* McItem::mIronHorseArmor = nullptr;
McItem* McItem::mGoldHorseArmor = nullptr;
McItem* McItem::mDiamondHorseArmor = nullptr;
McItem* McItem::mLead = nullptr;

McItem* McItem::mLeather = nullptr;
McItem* McItem::mRabbitHide = nullptr;
//McItem* McItem::milk = nullptr;
McItem* McItem::mBrick = nullptr;
McItem* McItem::mClay = nullptr;
McItem* McItem::mReeds = nullptr;
McItem* McItem::mPaper = nullptr;
McItem* McItem::mBook = nullptr;
McItem* McItem::mSlimeBall = nullptr;
McItem* McItem::mChestMinecart = nullptr;
//McItem* McItem::minecart_furnace = nullptr;
McItem* McItem::mEgg = nullptr;
McItem* McItem::mCompass = nullptr;
McItem* McItem::mFishingRod = nullptr;
McItem* McItem::mClock = nullptr;
McItem* McItem::mYellowDust = nullptr;
McItem* McItem::mCarrotOnAStick = nullptr;

McItem* McItem::mFish_raw_cod = nullptr;			
McItem* McItem::mFish_raw_salmon = nullptr;			
McItem* McItem::mFish_raw_clownfish = nullptr;		
McItem* McItem::mFish_raw_pufferfish = nullptr;		

McItem* McItem::mFish_cooked_cod = nullptr;			
McItem* McItem::mFish_cooked_salmon = nullptr;		

McItem* McItem::mMelon = nullptr;

McItem* McItem::mSpeckled_melon = nullptr;

McItem* McItem::mSeeds_pumpkin = nullptr;
McItem* McItem::mSeeds_melon = nullptr;

McItem* McItem::mDye_powder = nullptr;
McItem* McItem::mBone = nullptr;
McItem* McItem::mSugar = nullptr;
McItem* McItem::mCake = nullptr;

McItem* McItem::mBed = nullptr;
McItem* McItem::mRepeater = nullptr;
McItem* McItem::mCookie = nullptr;

McItem* McItem::mPotato = nullptr;
McItem* McItem::mPotatoBaked = nullptr;
McItem* McItem::mCarrot = nullptr;
McItem* McItem::mGoldenCarrot = nullptr;
McItem* McItem::mBeetroot = nullptr;
McItem* McItem::mSeeds_beetroot = nullptr;
McItem* McItem::mBeetrootSoup = nullptr;

McItem* McItem::mSpider_eye = nullptr;
McItem* McItem::mFermented_spider_eye = nullptr;
McItem* McItem::mPoisonous_potato = nullptr;

McItem* McItem::mSkull = nullptr;

McItem* McItem::mEnderpearl = nullptr;
McItem* McItem::mEnderEye = nullptr;
McItem* McItem::mEndCrystal = nullptr;
McItem* McItem::mBlazeRod = nullptr;
McItem* McItem::mNether_wart = nullptr;
McItem* McItem::mGold_nugget = nullptr;
McItem* McItem::mBlazePowder = nullptr;

//McItem* McItem::repeater = nullptr;
McItem* McItem::mNetherStar = nullptr;

McItem* McItem::mShears = nullptr;
McItem* McItem::mBeef_raw = nullptr;
McItem* McItem::mBeef_cooked = nullptr;
McItem* McItem::mChicken_raw = nullptr;
McItem* McItem::mChicken_cooked = nullptr;
McItem* McItem::mMutton_raw = nullptr;
McItem* McItem::mMutton_cooked = nullptr;
McItem* McItem::mRotten_flesh = nullptr;
McItem* McItem::mRabbitRaw = nullptr;
McItem* McItem::mRabbitCooked = nullptr;
McItem* McItem::mRabbitStew = nullptr;
McItem* McItem::mRabbitFoot = nullptr;

McItem* McItem::mNameTag = nullptr;

McItem* McItem::mDoor_spruce = nullptr;
McItem* McItem::mDoor_birch = nullptr;
McItem* McItem::mDoor_jungle = nullptr;
McItem* McItem::mDoor_acacia = nullptr;
McItem* McItem::mDoor_darkoak = nullptr; 

McItem* McItem::mGhast_tear = nullptr;

McItem* McItem::mMagma_cream = nullptr;
McItem* McItem::mBrewing_stand = nullptr;
McItem* McItem::mCauldron = nullptr;

McItem* McItem::mEnchanted_book = nullptr;
McItem* McItem::mComparator = nullptr;
McItem* McItem::mNetherbrick = nullptr;
McItem* McItem::mNetherQuartz = nullptr;
McItem* McItem::mTNTMinecart = nullptr;

McItem* McItem::mPrismarineShard = nullptr;
McItem* McItem::mPrismarineCrystals = nullptr;

McItem* McItem::mEmerald = nullptr;

McItem* McItem::mItemFrame = nullptr;

McItem* McItem::mFlowerPot = nullptr;

McItem* McItem::mPotion = nullptr;
McItem* McItem::mSplash_potion = nullptr;
McItem* McItem::mLingering_potion = nullptr;
McItem* McItem::mGlass_bottle = nullptr;
McItem* McItem::mDragon_breath = nullptr;

McItem* McItem::mHopperMinecart = nullptr;
McItem* McItem::mHopper = nullptr;

McItem* McItem::mFilledMap = nullptr;
McItem* McItem::mEmptyMap = nullptr;

McItem* McItem::mPortfolioBook = nullptr;
McItem* McItem::mChalkboard = nullptr;
McItem* McItem::mCamera = nullptr;

McItem* McItem::mShulkerShell = nullptr;

void McItem::setTextureAtlas(std::shared_ptr<TextureAtlas> itemsTextureAtlas) {
	mItemTextureAtlas = itemsTextureAtlas;
}

template<class T, class ... Args>
T& registerItem(Args&& ... args) {
	auto item = make_unique<T>(std::forward<Args>(args) ...);

	short actualId = item->getId();
	std::string descId = Util::toLower(item->getRawNameId());

	DEBUG_ASSERT(actualId >= 0 && actualId <= McItem::MAX_ITEMS, "Invalid ID value");
	DEBUG_ASSERT(!item->getDescriptionId().empty(), "McItem missing a name?! please fix");
	DEBUG_ASSERT(McItem::mItems[actualId] == nullptr, "ID Already taken! Please fix!");

	// Id lookup
	McItem::mItems[actualId] = item.get();
	
	// Name lookup
	McItem::mItemLookupMap[descId] = std::move(item);

	return static_cast<T&>(*McItem::mItems[actualId]);
}

int itemVarientFromString(const std::string& str) {
	if (str.empty()) {
		return 0;
	}
	
	// Lazy Init of static lookup map
	static std::unordered_map<std::string, int> LOOKUP_MAP;
	if (LOOKUP_MAP.empty()) {
		LOOKUP_MAP["wood"] = 0;
		LOOKUP_MAP["stone"] = 1;
		LOOKUP_MAP["iron"] = 2;
		LOOKUP_MAP["gold"] = 3;
		LOOKUP_MAP["diamond"] = 4;
	}

	// Lookup case insensitive by using *lower case* keys.
	auto i = LOOKUP_MAP.find(Util::toLower(str));
	return i != LOOKUP_MAP.end() ? i->second : 0;
}

template<class T>
std::unique_ptr<T> registerComponent(McItem& item, Json::Value & data, const std::string & componentName) {

	// Is the component data there?
	Json::Value & componentData = data[componentName];
	if (componentData.isNull())
		return std::unique_ptr<T>();

	// Create component
	std::unique_ptr<T> component = make_unique<T>(item);
	if (!component->init(componentData))
		return std::unique_ptr<T>();

	return std::move(component);
}

/*static*/
void McItem::registerItems() {
	// Set static pointers
	McItem::mApple = &registerItem<McItem>("apple", 4);
	McItem::mApple_gold = &registerItem<McItem>("golden_apple", 66);
	McItem::mApple_enchanted = &registerItem<McItem>("appleEnchanted", 210); //Parity: Separate item instead of data value 1 on golden apple
	McItem::mMushroomStew = &registerItem<McItem>("mushroom_stew", 26);
	McItem::mBread = &registerItem<McItem>("bread", 41);
	McItem::mPorkChop_raw = &registerItem<McItem>("porkchop", 63);
	McItem::mPorkChop_cooked = &registerItem<McItem>("cooked_porkchop", 64);

	McItem::mFish_raw_cod = &registerItem<McItem>("fish", 93); //Parity: Fish are separate items instead of augmented by data value.
	McItem::mFish_raw_salmon = &registerItem<McItem>("salmon", 204);
	McItem::mFish_raw_clownfish = &registerItem<McItem>("clownfish", 205);
	McItem::mFish_raw_pufferfish = &registerItem<McItem>("pufferfish", 206);
	McItem::mFish_cooked_cod = &registerItem<McItem>("cooked_fish", 94);
	McItem::mFish_cooked_salmon = &registerItem<McItem>("cooked_salmon", 207);

	McItem::mCookie = &registerItem<McItem>("cookie", 101);
	McItem::mMelon = &registerItem<McItem>("melon", 104);
	McItem::mBeef_raw = &registerItem<McItem>("beef", 107);
	McItem::mBeef_cooked = &registerItem<McItem>("cooked_beef", 108);
	McItem::mChicken_raw = &registerItem<McItem>("chicken", 109);
	McItem::mChicken_cooked = &registerItem<McItem>("cooked_chicken", 110);
	McItem::mMutton_raw = &registerItem<McItem>("muttonRaw", 167);
	McItem::mMutton_cooked = &registerItem<McItem>("muttonCooked", 168);
	McItem::mRotten_flesh = &registerItem<McItem>("rotten_flesh", 111);
	McItem::mSpider_eye = &registerItem<McItem>("spider_eye", 119);
	McItem::mCarrot = &registerItem<McItem>("carrot", 135);
	McItem::mPotato = &registerItem<McItem>("potato", 136);
	McItem::mPotatoBaked = &registerItem<McItem>("baked_potato", 137);
	McItem::mPoisonous_potato = &registerItem<McItem>("poisonous_potato", 138);
	McItem::mGoldenCarrot = &registerItem<McItem>("golden_carrot", 140);
	McItem::mPumpkinPie = &registerItem<McItem>("pumpkin_pie", 144);
	McItem::mBeetroot = &registerItem<McItem>("beetroot", 201);
	McItem::mBeetrootSoup = &registerItem<McItem>("beetroot_soup", 203);
	McItem::mRabbitRaw = &registerItem<McItem>("rabbit", 155);
	McItem::mRabbitCooked = &registerItem<McItem>("cooked_rabbit", 156);
	McItem::mRabbitStew = &registerItem<McItem>("rabbit_stew", 157);

	McItem::mSeeds_wheat = &registerItem<McItem>("wheat_seeds", 39);
	McItem::mSeeds_pumpkin = &registerItem<McItem>("pumpkin_seeds", 105);
	McItem::mSeeds_melon = &registerItem<McItem>("melon_seeds", 106);
	McItem::mNether_wart = &registerItem<McItem>("nether_wart", 116);
	McItem::mSeeds_beetroot = &registerItem<McItem>("beetroot_seeds", 202);

	McItem::mPrismarineShard = &registerItem<McItem>("prismarine_shard", 153);
	McItem::mPrismarineCrystals = &registerItem<McItem>("prismarine_crystals", 166);

	McItem::mShulkerShell = &registerItem<McItem>("shulker_shell", 189);

	// EDU Items
#ifdef MCPE_EDU
	McItem::mChalkboard = &registerItem<ChalkboardItem>("board", 198).setCategory(CreativeItemCategory::Decorations);
	McItem::mCamera = &registerItem<McItem>("camera", 242).setIsMirroredArt(true);
	McItem::mPortfolioBook = &registerItem<PortfolioBookItem>("portfolio", 200);
#endif
}

void McItem::addCreativeItem(short item, short auxValue) {
	addCreativeItem(ItemInstance(item, 1, auxValue));
}

void McItem::addCreativeItem(const Block* block, short auxValue) {
	addCreativeItem(ItemInstance(block->mID, 1, auxValue));
}

void McItem::addCreativeItem(McItem* item, short auxValue) {
	addCreativeItem(ItemInstance(item->getId(), 1, auxValue));
}

void McItem::addCreativeItem(const ItemInstance& itemInstance) {
	mCreativeList.emplace_back(itemInstance);
}

/*static*/
void McItem::teardownItems() {
	for (int i = 0; i < MAX_ITEMS; ++i) {
		McItem::mItems[i] = nullptr;
	}
	McItem::mItemLookupMap.clear();
}

float McItem::destroySpeedBonus(const ItemInstance* inst) const {
	if (!inst) {
		return 1.0f;
	}

// 	int efficiency = EnchantUtils::getEnchantLevel(Enchant::Type::MINING_EFFICIENCY, *inst);
	int efficiency = 0;
	if (efficiency <= 0) {
		return 0.0f;
	}

	return (float)efficiency * efficiency + 1;
}

//
// McItem impl.
//
McItem::McItem(const std::string & nameId, short id)
	: mId(256 + id)
	, mDescriptionId(ICON_DESCRIPTION_PREFIX + nameId)
	, mRawNameId(nameId)
	, mCraftingRemainingItem(nullptr)
	, mMaxDamage(0)
	, mIconTexture(nullptr)
	, mIconAtlas(nullptr)
	, mCreativeCategory(CreativeItemCategory::Miscellaneous)
	, mHandEquipped(false)
	, mIsStackedByData(false)
	, mIsGlint(false)
	, mShouldDespawn(true)
	, mIsMirroredArt(false)
	, mRequiresWorldBuilder(false)
	, mMaxUseDuration(0)
	, mBlockId(BlockID::AIR)
// 	, mUseAnim(UseAnimation::None)
	, mHoverTextColorFormat(nullptr)
	, mExplodable(true)
	, mUsesRenderingAdjustment(false)
	, mRenderingAdjTrans(0)
	, mRenderingAdjRot(0)
	, mRenderingAdjScale(1.0f) {

	DEBUG_ASSERT(!nameId.empty(),"missing a name for this item");
}

byte McItem::getMaxStackSize(const ItemInstance* item) {
	return m_maxStackSize;
}

void McItem::inventoryTick(ItemInstance& itemInstance, Level& level, Entity& owner, int slot, bool selected) {
}

void McItem::onCraftedBy(ItemInstance& itemInstance, Level& level, Player& player) {
}

CooldownType McItem::getCooldownType() const {
	return CooldownType::None;
}

int McItem::getCooldownTime() const {
	return 0;
}

std::string McItem::getInteractText(const Player& player) const {
	std::string ret = "";
// 	if (player.getBoostableComponent()) {
// 		if (player.getBoostableComponent()->itemUseText(player, ret)) {
// 			return ret;
// 		}
// 	}

	return ret;
}

McItem& McItem::setMaxStackSize(byte max) {
	m_maxStackSize = max;
	return *this;
}

McItem& McItem::setRequiresWorldBuilder(bool value) {
	mRequiresWorldBuilder = value;
	return *this;
}

bool McItem::canBeDepleted() {
	return mMaxDamage > 0 && !mIsStackedByData;
}

const TextureUVCoordinateSet& McItem::getIcon(int auxValue, int frame, bool inInventoryPane) const {

	// Prefer UVTextureItem over mIcon if set
	if (mIconAtlas != nullptr)
	{
		int clampedValue = Math::clamp(auxValue, 0, 15);
		return (*mIconAtlas)[clampedValue];
	}

	DEBUG_ASSERT(mIconTexture,"McItem doesn't have an icon atlas so it must have a single icon texture. If not we got bad data baby, bad data.");
	return *mIconTexture;
}

McItem& McItem::setIcon(TextureUVCoordinateSet const& icon) {
	mIconTexture = &icon;
	return *this;
}

McItem& McItem::setIcon(const std::string& name, int id) {

	mIconTexture = &mItemTextureAtlas->getTextureItem(name)[id];
	return *this;
}

BlockShape McItem::getBlockShape() const {
	return BlockShape::INVISIBLE;
}

bool McItem::isMirroredArt() const {
	return mIsMirroredArt;
}

McItem& McItem::setIsMirroredArt(bool val) {
	mIsMirroredArt = val;
	return *this;
}

ItemInstance& McItem::use(ItemInstance& itemInstance, Player& player) {
	return itemInstance;
}

bool McItem::useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) {
	return false;
}

float McItem::getDestroySpeed(ItemInstance* itemInstance, const Block* block) {
	return 1;
}

bool McItem::canDestroySpecial(const Block* block) const {
	return false;
}

int McItem::getLevelDataForAuxValue(int auxValue) const {
	return 0;
}

bool McItem::isStackedByData() const {
	return mIsStackedByData;
}

McItem& McItem::setCategory(CreativeItemCategory creativeCategory) {
	mCreativeCategory = creativeCategory;
	return *this;
}

short McItem::getMaxDamage() {
	return mMaxDamage;
}

int McItem::getAttackDamage() {
	return 0;
}

void McItem::hurtEnemy(ItemInstance* itemInstance, Mob* mob, Mob* attacker) {
	//unless overrided, all tools take 2 damage on durability( not on proper uses )
// 	itemInstance->hurtAndBreak(2, attacker);
}

bool McItem::interactEnemy(ItemInstance* itemInstance, Mob* mob, Player* player) {
	return false;
}

bool McItem::mineBlock(ItemInstance* itemInstance, BlockID blockId, int x, int y, int z, Entity* owner) {
	
	//unless overrided, all tools take 2 damage on durability( not on proper uses )
	const Block* block = Block::mBlocks[blockId];
	if (block && block->canHurtAndBreakItem()) {
		itemInstance->hurtAndBreak(2, owner);
	}

	return true; //??Never used
}

McItem& McItem::setHandEquipped() {
	mHandEquipped = true;
	return *this;
}

bool McItem::isHandEquipped() const {
	return mHandEquipped;
}

bool McItem::isArmor() const {
	return false;
}

bool McItem::isDye() const {
	return false;
}

bool McItem::isGlint(const ItemInstance* itemInstance) const {
	
	if (itemInstance && itemInstance->isEnchanted())
		return true;

	return mIsGlint;
}

bool McItem::isThrowable() const {
	return false;
}

bool McItem::canDestroyInCreative() const {
	return true;
}

std::string McItem::buildDescriptionName(const ItemInstance& itemInstance) const {
	
	std::string out;
	out = I18n::get(mDescriptionId + ".name");
	return out;
}

std::string McItem::buildEffectDescriptionName(const ItemInstance& instance) const {
	return {};
}

McItem& McItem::setStackedByData(bool isStackedByData) {
	mIsStackedByData = isStackedByData;
	return *this;
}

McItem& McItem::setMaxDamage(int maxDamage) {
	mMaxDamage = maxDamage;
	return *this;
}

TextureUVCoordinateSet McItem::getTextureUVCoordinateSet(const std::string& name, int id) {
	return mItemTextureAtlas->getTextureItem(name)[id];
}

const TextureAtlasItem& McItem::getTextureItem(const std::string& name) {
	return mItemTextureAtlas->getTextureItem(name);
}

int McItem::getEnchantSlot(void) const {
	return 0;
}

// an already enchanted book cannot be randomly enchanted using an enchanting table
int McItem::getEnchantValue(void) const {
	return (mId == McItem::mBook->getId()) ? 1 : 0;
}

bool McItem::isComplex() const {
	return false;
}

int McItem::getDamageChance(int unbreaking) const {
	return 100 / (unbreaking + 1);
}

McItem* McItem::lookupByName(const std::string& name, bool caseInsensitive) {
	DEBUG_ASSERT(!name.empty(),"must pass in a non empty string for item Name");

	// All names in map are lower case (so input must always be lower case)
	std::string testName = caseInsensitive ? Util::toLower(name) : name;

	auto itr = mItemLookupMap.find(testName);
	if (itr != mItemLookupMap.end()) {
		return itr->second.get();
	}
	
	// See if we need to pre pend "item." (happens with text coming from console commands)
	if (testName.find("item.") == -1) {
		testName = "item." + testName;
		itr = mItemLookupMap.find(testName);
		if (itr != mItemLookupMap.end()) {
			return itr->second.get();
		}
	}

	//TODO adors
	//hack for updating to new format
	if (name.find("minecraft:") == 0) {
		testName = name.substr(std::string("minecraft:").length());
		itr = mItemLookupMap.find(testName);
		if (itr != mItemLookupMap.end()) {
			return itr->second.get();
		}
	}

	return nullptr;
}

bool McItem::_textMatch(const std::string& n1, const std::string& n2, bool ignoreCaseFor1) {
	return ignoreCaseFor1 ? Util::toLower(n1) == n2 : n1 == n2;
}
