/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class DispenserBlock : public EntityBlock {
public:
	static const int FACING_MASK = 0x7;
	static const int TRIGGER_BIT = 8;

	DispenserBlock(const std::string& nameId, int id);

	int getVariant(int data) const override;
	FacingID getMappedFace(FacingID face, int data) const override;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	bool isInteractiveBlock() const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	ItemInstance getSilkTouchItemInstance(DataID data) const override;

	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	bool hasComparatorSignal() const override;

	virtual int getTickDelay() const;
protected:
	Vec3 getDispensePosition(BlockSource& region, const Vec3& pos) const;

	FacingID getFacing(int data) const;

private:
	void recalcLockDir(BlockSource& region, const BlockPos& pos);
};
