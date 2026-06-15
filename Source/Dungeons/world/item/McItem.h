/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

// Physical Dependencies
#include "locale/I18n.h"
#include "util/Random.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "client/renderer/block/BlockGraphics.h"
#include "ItemCategory.h"
#include "legacy/Core/Math/Color.h"

// Logical Dependencies
class TextureAtlas;
class Level;
class Block;
class Entity;
class Mob;
class Player;
class McItem;
class BlockPos;
class BlockSource;
class ItemInstance;
class Vec3;
class TextureAtlasItem;
enum class UseAnimation : byte;
class IDataInput;
class IDataOutput;

enum class CooldownType {
	None = -1,
	ChorusFruit = 0,
	EnderPearl = 1,

	Count
};

// [Constant McItem Definition class]
class McItem {
	static const byte MAX_STACK_SIZE = 64;	//Container::LARGE_MAX_STACK_SIZE;

public:
	static const int MAX_ITEMS = 512;//32000;
	static const int ICON_COLUMNS = 16;
	static const std::string ICON_DESCRIPTION_PREFIX;

	class Tier {
		const int mLevel;
		const int mUses;
		const float mSpeed;
		const int mDamage;
		const int mEnchantmentValue;
	public:
		static const Tier WOOD;
		static const Tier STONE;
		static const Tier IRON;
		static const Tier DIAMOND;
		static const Tier GOLD;

		Tier(int level, int uses, float speed, int damage, int enchant) :
			mLevel(level)
			, mUses(uses)
			, mSpeed(speed)
			, mDamage(damage)
			, mEnchantmentValue(enchant) {
		}

		int getUses() const { return mUses; }
		float getSpeed() const { return mSpeed; }
		int getAttackDamageBonus() const { return mDamage; }
		int getLevel() const { return mLevel; }
		int getEnchantmentValue() const { return mEnchantmentValue; }
		ItemInstance getTierItem(void) const;
	};

public:

	static McItem* mItems[MAX_ITEMS];
	static std::unordered_map<std::string, Unique<McItem>> mItemLookupMap;

	static McItem* mShovel_iron;
	static McItem* mPickAxe_iron;
	static McItem* mHatchet_iron;
	static McItem* mFlintAndSteel;
	static McItem* mApple;
	static McItem* mBow;
	static McItem* mArrow;
	static McItem* mCoal;
	static McItem* mDiamond;
	static McItem* mIronIngot;
	static McItem* mGoldIngot;
	static McItem* mSword_iron;

	static McItem* mSword_wood;
	static McItem* mShovel_wood;
	static McItem* mPickAxe_wood;
	static McItem* mHatchet_wood;

	static McItem* mSword_stone;
	static McItem* mShovel_stone;
	static McItem* mPickAxe_stone;
	static McItem* mHatchet_stone;

	static McItem* mSword_diamond;
	static McItem* mShovel_diamond;
	static McItem* mPickAxe_diamond;
	static McItem* mHatchet_diamond;

	static McItem* mStick;
	static McItem* mBowl;
	static McItem* mMushroomStew;

	static McItem* mSword_gold;
	static McItem* mShovel_gold;
	static McItem* mPickAxe_gold;
	static McItem* mHatchet_gold;

	static McItem* mString;
	static McItem* mFeather;
	static McItem* mSulphur;

	static McItem* mHoe_wood;
	static McItem* mHoe_stone;
	static McItem* mHoe_iron;
	static McItem* mHoe_diamond;
	static McItem* mHoe_gold;

	static McItem* mSeeds_wheat;
	static McItem* mWheat;
	static McItem* mBread;

	static McItem* mHelmet_cloth;
	static McItem* mChestplate_cloth;
	static McItem* mLeggings_cloth;
	static McItem* mBoots_cloth;

	static McItem* mElytra;

	static McItem* mHelmet_chain;
	static McItem* mChestplate_chain;
	static McItem* mLeggings_chain;
	static McItem* mBoots_chain;

	static McItem* mHelmet_iron;
	static McItem* mChestplate_iron;
	static McItem* mLeggings_iron;
	static McItem* mBoots_iron;

	static McItem* mHelmet_diamond;
	static McItem* mChestplate_diamond;
	static McItem* mLeggings_diamond;
	static McItem* mBoots_diamond;

	static McItem* mHelmet_gold;
	static McItem* mChestplate_gold;
	static McItem* mLeggings_gold;
	static McItem* mBoots_gold;

	static McItem* mFlint;
	static McItem* mPorkChop_raw;
	static McItem* mPorkChop_cooked;
	static McItem* mPainting;

	static McItem* mChorusFruit;
	static McItem* mChorusFruitPopped;

	static McItem* mApple_gold;
	static McItem* mApple_enchanted;

	static McItem* mSign;
	static McItem* mDoor_wood;

	static McItem* mBucket;

	static McItem* mMinecart;
	static McItem* mSaddle;
	static McItem* mDoor_iron;
	static McItem* mRedStone;
	static McItem* mSnowBall;

	static McItem* mBoat;

	static McItem* mLeather;
	static McItem* mRabbitHide;

	//static McItem* milk;
	static McItem* mBrick;
	static McItem* mClay;
	static McItem* mReeds;
	static McItem* mPaper;
	static McItem* mBook;
	static McItem* mSlimeBall;
	static McItem* mChestMinecart;
	static McItem* mMinecart_chest;
	static McItem* mMinecart_furnace;
	static McItem* mEgg;
	static McItem* mCompass;
	static McItem* mFishingRod;
	static McItem* mClock;
	static McItem* mYellowDust;
	static McItem* mCarrotOnAStick;

	static McItem* mFish_raw_cod;
	static McItem* mFish_raw_salmon;
	static McItem* mFish_raw_clownfish;
	static McItem* mFish_raw_pufferfish;
	static McItem* mFish_cooked_cod;
	static McItem* mFish_cooked_salmon;

	static McItem* mMelon;
	static McItem* mSpeckled_melon;
	static McItem* mSeeds_pumpkin;
	static McItem* mSeeds_melon;

	static McItem* mEnderpearl;
	static McItem* mEnderEye;
	static McItem* mEndCrystal;
	static McItem* mBlazeRod;
	static McItem* mNether_wart;
	static McItem* mGold_nugget;
	static McItem* mSpider_eye;
	static McItem* mFermented_spider_eye;
	static McItem* mBlazePowder;

	static McItem* mMobPlacer;
	static McItem* mExperiencePotionItem;

	static McItem* mFireCharge;

	static McItem* mDye_powder;
	static McItem* mBone;
	static McItem* mSugar;
	static McItem* mCake;

	static McItem* mBed;
	static McItem* mRepeater;
	static McItem* mCookie;

	static McItem* mPumpkinPie;

	static McItem* mPotato;
	static McItem* mPotatoBaked;
	static McItem* mPoisonous_potato;
	static McItem* mCarrot;
	static McItem* mGoldenCarrot;
	static McItem* mBeetroot;
	static McItem* mSeeds_beetroot;
	static McItem* mBeetrootSoup;

	static McItem* mSkull;

	static McItem* mNetherStar;

	static McItem* mDiode;

	static McItem* mShears;

	static McItem* mBeef_raw;
	static McItem* mBeef_cooked;
	static McItem* mChicken_raw;
	static McItem* mChicken_cooked;
	static McItem* mMutton_raw;
	static McItem* mMutton_cooked;
	static McItem* mRotten_flesh;

	static McItem* mRabbitRaw;
	static McItem* mRabbitCooked;
	static McItem* mRabbitStew;
	static McItem* mRabbitFoot;

	static McItem* mLeatherHorseArmor;
	static McItem* mIronHorseArmor;
	static McItem* mGoldHorseArmor;
	static McItem* mDiamondHorseArmor;
	static McItem* mLead;
	static McItem* mNameTag;

	static McItem* mDoor_spruce;
	static McItem* mDoor_birch;
	static McItem* mDoor_jungle;
	static McItem* mDoor_acacia;
	static McItem* mDoor_darkoak;

	static McItem* mGhast_tear;

	static McItem* mMagma_cream;
	static McItem* mBrewing_stand;
	static McItem* mCauldron;

	static McItem* mEnchanted_book;
	static McItem* mComparator;
	static McItem* mNetherbrick;
	static McItem* mNetherQuartz;
	static McItem* mTNTMinecart;

	static McItem* mRecord_01;
	static McItem* mRecord_02;

	static McItem* mPrismarineShard;
	static McItem* mPrismarineCrystals;
	
	static McItem* mEmerald;
	static McItem* mItemFrame;

	static McItem* mFlowerPot;

	static McItem* mPotion;
	static McItem* mSplash_potion;
	static McItem* mLingering_potion;
	static McItem* mGlass_bottle;
	static McItem* mDragon_breath;

	static McItem* mHopperMinecart;
	static McItem* mHopper;

	static McItem* mFilledMap;
	static McItem* mEmptyMap;

	static McItem* mPortfolioBook;
	static McItem* mChalkboard;
	static McItem* mCamera;

	static McItem* mShulkerShell;

	static void setTextureAtlas(std::shared_ptr<TextureAtlas> itemsTextureAtlas);

	static void registerItems();		// Create all the items.
	static void addCreativeItem(short id, short auxValue = 0);
	static void addCreativeItem(const Block* block, short auxValue = 0);
	static void addCreativeItem(McItem* item, short auxValue = 0);
	static void addCreativeItem(const ItemInstance& instance);
	static void teardownItems();

protected:

	//OVERRIDABLES
	//these members can (and should) be set to non-default values by implementing items
	byte m_maxStackSize = MAX_STACK_SIZE;
	std::string m_textureAtlasFile = "atlas.items";
	int m_frameCount = 1;
	bool m_animatesInToolbar = false;

	float destroySpeedBonus(const ItemInstance* inst) const;

public:

	// Construction/Destruction
	McItem(const std::string& nameId, short id);
	virtual ~McItem() { }

	// Field Access
	short getId() const { return mId; }
	const std::string & getDescriptionId() const { return mDescriptionId; }
	const std::string& getRawNameId() const { return mRawNameId; }
	const TextureUVCoordinateSet* getIconTexture() const { return mIconTexture; }
	const TextureAtlasItem* getIconAtlas() const { return mIconAtlas; }
	int getFrameCount() const { return m_frameCount; }
	bool isAnimatedInToolbar() const { return m_animatesInToolbar; }
	UseAnimation getUseAnimation() const { return mUseAnim; }
	int getMaxUseDuration() const { return mMaxUseDuration; }
	CreativeItemCategory getCreativeCategory() const { return mCreativeCategory; }
	BlockID getBlockId() const { return mBlockId; }
	bool isExplodable() const { return mExplodable; }
	bool shouldDespawn() const { return mShouldDespawn; }

	// rendering adjustments access
	// currently, these only affect VR reality mode, but could be extended later for other situations
	bool usesRenderingAdjustment() const { return mUsesRenderingAdjustment; }
	Vec3 getRenderingPosAdjustment() const { return mRenderingAdjTrans; }
	Vec3 getRenderingRotAdjustment() const { return mRenderingAdjRot; }
	float getRenderingScaleAdjustment() const { return mRenderingAdjScale; }

	// Settings API
	virtual McItem& setIcon(const std::string& name, int id = 0);
	virtual McItem& setIcon(TextureUVCoordinateSet const& icon);
	virtual McItem& setMaxStackSize(byte max);
	virtual McItem& setCategory(CreativeItemCategory creativeCategory);
	virtual McItem& setStackedByData(bool isStackedByData);
	virtual McItem& setMaxDamage(int maxDamage);
	virtual McItem& setHandEquipped();
	virtual McItem& setUseAnimation(UseAnimation anim) { mUseAnim = anim; return *this; }
	virtual McItem& setMaxUseDuration(int maxUseDuration) { mMaxUseDuration = maxUseDuration; return *this; }
	virtual McItem& setRequiresWorldBuilder(bool value = true);
	virtual McItem& setExplodable(bool boom = true) { mExplodable = boom; return *this; }
	virtual McItem& setIsGlint(bool glint = false) { mIsGlint = glint; return *this; }
	virtual McItem& setShouldDespawn(bool despawn = true) { mShouldDespawn = despawn; return *this; }
	virtual BlockShape getBlockShape() const;
	virtual bool canBeDepleted();
	virtual bool canDestroySpecial(const Block* block) const;
	virtual int getLevelDataForAuxValue(int auxValue) const;
	virtual bool isStackedByData() const;
	virtual short getMaxDamage();
	virtual int getAttackDamage();
	virtual bool isHandEquipped() const;
	virtual bool isArmor() const;
	virtual bool isDye() const;
	virtual bool isGlint(const ItemInstance* instance) const;
	virtual bool isThrowable() const;
	virtual bool canDestroyInCreative() const;
	virtual bool isLiquidClipItem(int auxValue) const {
		UNUSED_PARAMETER(auxValue);
		return false; 
	}
	virtual bool requiresInteract() const { return false; }
	virtual void appendFormattedHovertext(const ItemInstance& item, Level& level, std::string& hovertext, const bool advancedToolTops) const {}
	virtual bool isValidRepairItem(const ItemInstance& source, const ItemInstance& repairItem) {
		UNUSED_PARAMETER(source,repairItem);
		return false; 
	}
	virtual int getEnchantSlot(void) const;
	virtual int getEnchantValue(void) const;
	virtual bool isComplex() const;
	virtual bool isValidAuxValue(int auxValue) const {
		UNUSED_PARAMETER(auxValue);
		return true; 
	}

	virtual int getDamageChance(int unbreaking) const;

	// Used to tell if Aux values are used to indicate a completely different version of an item such as potions
	virtual bool uniqueAuxValues() const { return false; }

	virtual Color getColor(const ItemInstance& instance) const {
		UNUSED_PARAMETER(instance);
		return Color::WHITE;
	}

	// Instance API
	virtual ItemInstance& use(ItemInstance& instance, Player& player);
	virtual bool useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ);
	virtual void useTimeDepleted(ItemInstance* inoutInstance, Level* level, Player* player) {}
	virtual void releaseUsing(ItemInstance* instance, Player* player, int durationLeft) {}
	virtual float getDestroySpeed(ItemInstance* instance, const Block* block);
	virtual void hurtEnemy(ItemInstance* instance, Mob* mob, Mob* attacker);
	virtual bool interactEnemy(ItemInstance* instance, Mob* mob, Player* player);
	virtual bool mineBlock(ItemInstance* itemInstance, BlockID blockId, int x, int y, int z, Entity* owner);
	virtual std::string buildDescriptionName(const ItemInstance& instance) const;
	virtual std::string buildEffectDescriptionName(const ItemInstance& instance) const;
	virtual byte getMaxStackSize(const ItemInstance* item);
	virtual void inventoryTick(ItemInstance& itemInstance, Level& level, Entity& owner, int slot, bool selected);
	virtual void onCraftedBy(ItemInstance& itemInstance, Level& level, Player& player);
	virtual CooldownType getCooldownType() const;
	virtual int getCooldownTime() const;

	//graphical component //TODO extract all these to ItemGraphics
	virtual std::string getInteractText(const Player& player) const;
	virtual int getAnimationFrameFor(Mob& holder) const {
		UNUSED_PARAMETER(holder);
		return 0; 
	}
	virtual bool isEmissive(int data) const {
		UNUSED_PARAMETER(data);
		return false; 
	}
	virtual const TextureUVCoordinateSet& getIcon(int auxValue, int frame, bool inInventoryPane = false) const;
	virtual int getIconYOffset() const { return 0; }
	virtual bool isMirroredArt() const;
	McItem& setIsMirroredArt(bool val);
	//end graphical component

	static McItem* lookupByName(const std::string& name, bool caseInsensitive = true);
	static bool _textMatch(const std::string& n1, const std::string& n2, bool ignoreCaseFor1);
	static TextureUVCoordinateSet getTextureUVCoordinateSet(const std::string& name, int id = 0);
	static const TextureAtlasItem& getTextureItem(const std::string& name);

protected:
	// Fields
	const short mId;
	std::string mDescriptionId;
	std::string mRawNameId;
	bool mIsMirroredArt;
	short mMaxDamage;
	bool mIsGlint;
	bool mHandEquipped;
	bool mIsStackedByData;
	bool mRequiresWorldBuilder:1;
	int mMaxUseDuration;
	bool mExplodable;
	bool mShouldDespawn;
	BlockID mBlockId;
	UseAnimation mUseAnim;
	CreativeItemCategory mCreativeCategory;
	McItem* mCraftingRemainingItem;
	const std::string* mHoverTextColorFormat;
	const TextureUVCoordinateSet* mIconTexture;	// single uvs
	const TextureAtlasItem* mIconAtlas;	// multiple uvs

	// rendering adjustments
	bool mUsesRenderingAdjustment; // did the .json have a rendering adjustment?  Only apply adjustments if this is true

	// relative to the hand, X axis completes the right-handed frame
	// relative to the hand, if you hold a fist, the +Y axis is along the axis a sword would be pointing towards
	// relative to the hand, +Z is the direction pointed fingers would point in
	Vec3 mRenderingAdjTrans; 

	// rotation adjustments follow the right-handed rule:
	//  point thumb along axis, rotation towards the fingertips is positive angles
	//
	// note that these form Euler angles, and are applied in the following sequential order:
	// Y (rotates in the axis of the "thumb", think of the sword "twisting")
	// X (rotates in the plane of the thumb/fingers)
	// z (rotates along the fingers/forearm)
	Vec3 mRenderingAdjRot;

	// overall render scale adjustment, cumulative with other scale factors
	float mRenderingAdjScale;
public:
	// Statics
	static std::vector<ItemInstance> mCreativeList;
	static std::shared_ptr<TextureAtlas> mItemTextureAtlas;
	static Random mRandom;
};
