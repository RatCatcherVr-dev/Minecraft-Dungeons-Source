/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/item/ItemInstance.h"

#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/item/McItem.h"
#include "world/level/block/Block.h"
#include "world/level/Level.h"

const int ItemInstance::MAX_STACK_SIZE = 255;

ItemInstance::ItemInstance() {
	init(0, 0, 0);
}

ItemInstance::ItemInstance(bool valid_) {
	init(0, 0, 0);
	mValid = valid_;
}

ItemInstance::ItemInstance(const Block* block) {
	init(block->mID, 1, 0);
}

ItemInstance::ItemInstance(const Block* block, int count) {
	init(block->mID, count, 0);
}

ItemInstance::ItemInstance(const Block* block, int count, int auxValue) {
	init(block->mID, count, auxValue);
}

ItemInstance::ItemInstance(const McItem* item) {
	init(item->getId(), 1, 0);
}

ItemInstance::ItemInstance(const McItem* item, int count) {
	init(item->getId(), count, 0);
}

ItemInstance::ItemInstance(const McItem* item, int count, int auxValue) {
	init(item->getId(), count, auxValue);
}

ItemInstance::ItemInstance(int id, int count, int auxValue) {
	init(id, count, auxValue);
}

ItemInstance::ItemInstance(const ItemInstance& rhs) {
	init(rhs.getId(), rhs.mCount, rhs.mAuxValue);
}

bool ItemInstance::_setItem(int id) {

	mItem = (id >= 0 && id < McItem::MAX_ITEMS) ? McItem::mItems[id] : nullptr;

	// Block McItem's specify their own blockId.
	if (mItem != nullptr)
		id = mItem->getBlockId();

	mValid = mItem != nullptr || id == 0;	//id 0 is allowed to have a null item... not great but duh
	if (!mValid) {
		mItem = nullptr;
		mBlock = 0;
		mCount = 0;
		mAuxValue = -1;
	}
	else {
		mBlock = id > 0 && id < 256 ? Block::mBlocks[id] : nullptr;
	}

	return mValid;
}

void ItemInstance::init(int id, int count_, int aux_) {
	DEBUG_ASSERT(count_ >= 0, "A negative value will wrap around!");
	mCount = std::max(count_, 0);
	mAuxValue = aux_;
	_setItem(id);
}

bool ItemInstance::isNull() const {
	return !mValid || (mCount == 0 && mAuxValue == 0 && mItem == nullptr && mBlock == nullptr);
}

void ItemInstance::setNull() {
	mCount = 0;
	mAuxValue = 0;
	mItem = nullptr;
	mBlock = nullptr;
}

void ItemInstance::add(const int inCount) {
	set(mCount + inCount);
}

void ItemInstance::remove(int inCount) {
	set(mCount - inCount);
}

void ItemInstance::set(const int inCount) {
	DEBUG_ASSERT(inCount >= 0, "stack was negative");
	DEBUG_ASSERT(inCount <= getMaxStackSize(), "itemStack too big!");
	mCount = Math::clamp((byte)inCount, (byte)0, getMaxStackSize());

	if (isEmptyStack()) {
		setNull();
	}
}

void ItemInstance::onCraftedBy(Level& level, Player& player) {
	if (isItem()) {
		mItem->onCraftedBy(*this, level, player);
	}
}

bool ItemInstance::useOn(Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) {
	return mItem->useOn(*this, entity, x, y, z, face, clickX, clickY, clickZ);
}

bool ItemInstance::isGlint() const {
	return (mItem != nullptr) ? mItem->isGlint(this) : false;
}

int ItemInstance::retrieveIDFromIDAux(int idAux) {
	// AuxID is a 4-byte integer split into two halves
	// Left two bytes represents an Item's ID
	// Shift right by 16 bits and mask with ID Mask to retrieve Item's ID
	return ((idAux >> 16) & ID_MASK);
}

bool ItemInstance::retrieveEnchantFromIDAux(int idAux) {
	// Enchanted status is stored as a single-bit boolean flag held in the rightmost bit
	// Of the two rightmost bytes of the auxID
	// [byte] [byte] ([byte] [byte]) -- these two are the aux value and enchanted bit
	// [Exxx] [xxxx] [xxxx] [xxxx] -- E is the enchanted bit
	// To retrieve, use the Enchant mask
	return ((idAux & ENCHANT_MASK) == ENCHANT_MASK ? true : false);
}

int ItemInstance::retrieveAuxValFromIDAux(int idAux) {
	// Aux value is stored in the rightmost 2 bytes
	// To retrieve it properly we must ignore the enchanted status bit within those, however
	return (idAux & AUX_VALUE_MASK);
}

bool ItemInstance::isThrowable() const {
	return mItem && mItem->isThrowable();
}

float ItemInstance::getDestroySpeed(const Block* block) {
	return mItem->getDestroySpeed(this, block);
}

ItemInstance& ItemInstance::use(Player& player) {
	return *this;
}

void ItemInstance::inventoryTick(Level& level, Entity& owner, int slot, bool selected) {
	if (mItem != nullptr) {
		mItem->inventoryTick(*this, level, owner, slot, selected);
	}
}

void ItemInstance::useAsFuel() {
	if (mCount == 1 && mItem == McItem::mBucket && getAuxValue() == Block::mFlowingLava->mID) {//transform this item into an empty
																				// bucket
		*this = ItemInstance(mItem, 1, 0);
	}
	else {
		remove(1);
	}
}

bool ItemInstance::isEnchanted() const {
	return false;
}

int ItemInstance::getEnchantValue() const {
	return mItem ? mItem->getEnchantValue() : 0;
}

int ItemInstance::getEnchantSlot() const {
// 	return mItem ? mItem->getEnchantSlot() : Enchant::Slot::NONE;
	return mItem ? mItem->getEnchantSlot() : 0;
}

bool ItemInstance::isEnchantingBook() const {
	return isValid() && isItem() && mItem->getId() == McItem::mEnchanted_book->getId() && isEnchanted();
}

byte ItemInstance::getMaxStackSize() const {
	return mItem ? mItem->getMaxStackSize(this) : MAX_STACK_SIZE;
}

bool ItemInstance::isFullStack() const {
	return mCount >= getMaxStackSize();
}

bool ItemInstance::isStackable() const {
	return getMaxStackSize() > 1 && (!isDamageableItem() || !isDamaged());
}

bool ItemInstance::isStackable(const ItemInstance* a, const ItemInstance* b) {
	return a && b && a->mItem == b->mItem && b->isStackable()
		&& (!b->isStackedByData() || a->getAuxValue() == b->getAuxValue());
}

bool ItemInstance::isDamageableItem() const {
	return isItem() && mItem->getMaxDamage() > 0;
}

/**
 * Returns true if this item type only can be stacked with items that have
 * the same auxValue data.
 *
 * @return
 */
bool ItemInstance::isStackedByData() const {
	return mItem->isStackedByData();
}

bool ItemInstance::isDamaged() const {
	return isDamageableItem() && mAuxValue > 0;
}

short ItemInstance::getDamageValue() const {
	return mAuxValue;
}

short ItemInstance::getAuxValue() const {
	return mAuxValue;
}

void ItemInstance::setAuxValue(short value) {
	mAuxValue = value;
}

short ItemInstance::getMaxDamage() const {
	return mItem->getMaxDamage();
}

void ItemInstance::mineBlock(BlockID block, int x, int y, int z, Mob* owner) {
	mItem->mineBlock(this, block, x, y, z, owner);
}

int ItemInstance::getAttackDamage() {
	return mItem->getAttackDamage();
}

bool ItemInstance::canDestroySpecial(const Block* block) {
	return mItem->canDestroySpecial(block);
}

bool ItemInstance::interactEnemy(Mob* mob, Player* player) {
	return mItem->interactEnemy(this, mob, player);
}

/*static*/
bool ItemInstance::matches(const ItemInstance* a, const ItemInstance* b) {
	if (a == nullptr && b == nullptr) {
		return true;
	}
	if (a == nullptr || b == nullptr) {
		return false;
	}
	return a->matches(b);
}

/*static*/
bool ItemInstance::matchesNulls(const ItemInstance* a, const ItemInstance* b) {
	bool aNull = !a || a->isNull();
	bool bNull = !b || b->isNull();
	if (aNull && bNull) {
		return true;
	}
	if (aNull || bNull) {
		return false;
	}
	return a->matches(b);
}

/**
 * Checks if this item is the same item as the other one, disregarding the
 * 'count' value.
 *
 * @param b
 * @return
 */
bool ItemInstance::sameItemAndAux(const ItemInstance* b) const {
	return sameItem(b) && mAuxValue == b->mAuxValue;
}

ItemInstance* ItemInstance::setDescriptionId(const std::string& id) {
	return this;
}

std::string ItemInstance::getName() const {
	return mItem->buildDescriptionName(*this);
}

std::string ItemInstance::getRawNameId() const {
	std::string result;
	// Blocks are items and blocks, but the item raw name is tile.blockname where block is just blockname, so use block first
	if (mBlock) {
		result = mBlock->getRawNameId();
	}
	else if (mItem) {
		result = mItem->getRawNameId();
	}
	return result;
}

std::string ItemInstance::getEffectName() const {
	return mItem->buildEffectDescriptionName(*this);
}

std::string ItemInstance::getFormattedHovertext(Level& level, const bool advancedToolTops) const {
	std::string hoverText;
	if (isItem() && isValid()) {
		mItem->appendFormattedHovertext(*this, level, hoverText, advancedToolTops);
	}
	return hoverText;
}

int ItemInstance::getId() const {
	if (!mValid) {
		return -1;
	}
	else {
		return mItem ? mItem->getId() : (mBlock ? mBlock->mID : 0);
	}
}

int ItemInstance::getIdAux() const {
	// TODO breaks when types are changed
	DEBUG_ASSERT(mItem, "getting item ID without item.");

	return (mItem->getId() << 16) | (mAuxValue);
}

int ItemInstance::getIdAuxEnchanted() const {
	return getIdAux() | (isEnchanted() ? 1 << 15 : 0);
}

std::string ItemInstance::toString() const {
	std::stringstream ss;
	ss << mCount << " x " << getName() << "(" << mItem->getId() << ")" << "@" << mAuxValue;
	return ss.str();
}

/*static*/
ItemInstance* ItemInstance::clone(const ItemInstance* item) {
	return (item == nullptr || item->isNull()) ? nullptr : new ItemInstance(*item);
}

/*static*/
ItemInstance ItemInstance::cloneSafe(const ItemInstance* item) {
	return (item == nullptr || item->isNull()) ? ItemInstance() : *item;
}

/*private*/
bool ItemInstance::matches(const ItemInstance* b) const {
	return (mItem == b->mItem)
		&& (mCount == b->mCount)
		&& (mAuxValue == b->mAuxValue);
}

ItemInstance& ItemInstance::operator=(const ItemInstance& rhs) {
	mCount = rhs.mCount;
	mAuxValue = rhs.mAuxValue;
	mItem = rhs.mItem;
	mBlock = rhs.mBlock;
	mValid = rhs.mValid;

	return *this;
}

Color ItemInstance::getColor() const {
	return getItem()->getColor(*this);
}

bool ItemInstance::operator!=(const ItemInstance& rhs) const {
	return !matches(&rhs);
}

bool ItemInstance::operator==(const ItemInstance& rhs) const {
	return matches(&rhs);
}

const TextureUVCoordinateSet& ItemInstance::getIcon(int frame, bool inInventoryPane) const {
	return mItem->getIcon(mAuxValue, frame, inInventoryPane);
}

void ItemInstance::releaseUsing(Player* player, int durationLeft) {
	mItem->releaseUsing(this, player, durationLeft);
}

int ItemInstance::getMaxUseDuration() const {
	return mItem->getMaxUseDuration();
}

UseAnimation ItemInstance::getUseAnimation() const {
	return mItem->getUseAnimation();
}

void ItemInstance::useTimeDepleted(Level* level, Player* player) {
	return mItem->useTimeDepleted(this, level, player);
}

bool ItemInstance::isArmorItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	const McItem* item = instance->getItem();
	return item && item->isArmor();
}

bool ItemInstance::isHorseArmorItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	const McItem* item = instance->getItem();
	return item && (item == McItem::mLeatherHorseArmor || item == McItem::mIronHorseArmor || item == McItem::mGoldHorseArmor || item == McItem::mDiamondHorseArmor);;
}

bool ItemInstance::isWearableItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	return isArmorItem(instance) ||
		(instance->getBlock() == Block::mPumpkin) ||
		(instance->getItem() == McItem::mSkull);
}

bool ItemInstance::isPotionItem(const ItemInstance* instance) {
	return false;
}

bool ItemInstance::isItem(const ItemInstance* instance) {
	return instance && instance->getItem();
}

bool ItemInstance::isLiquidClipItem() {
	return mItem && mItem->isLiquidClipItem(mAuxValue);
}


int ItemInstance::getBaseRepairCost() const {
	return 0;
}
