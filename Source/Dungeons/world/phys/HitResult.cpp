#include "Dungeons.h"

#include "HitResult.h"
#include "world/entity/Entity.h"


HitResult::HitResult(const Vec3& rayEnd)
	:   mType(NO_HIT)
	, mFacing(0)
	, mEntity(nullptr)
	, mIsHitLiquid(false)
	, mIndirectHit(false)	
	, mPos(rayEnd) {
}

HitResult::HitResult() : HitResult(Vec3::ZERO) {}


HitResult::HitResult(const BlockPos& blockPos, FacingID facing, const Vec3& pos)
	: mType(TILE)
	, mFacing(facing)
	, mEntity(nullptr)
	, mIsHitLiquid(false)
	, mIndirectHit(false)
	, mPos(pos)
	, mBlock(blockPos)
{
}

HitResult::HitResult(Entity& entity)
	:   mType(ENTITY)
	, mFacing(0)
	, mEntity(&entity)
	, mIsHitLiquid(false)
	, mIndirectHit(false)
	, mPos(entity.mPos)
{
}


HitResult::HitResult(const HitResult& hr)
	: mType(hr.mType)
	, mFacing(hr.mFacing)
	, mLiquidFacing(hr.mLiquidFacing)
	, mEntity(hr.mEntity)
	, mIsHitLiquid(hr.mIsHitLiquid)
	, mIndirectHit(hr.mIndirectHit)
	, mPos(hr.mPos)
	, mLiquidPos(hr.mLiquidPos)
	, mBlock(hr.mBlock)
	, mLiquid(hr.mLiquid) {
}


HitResult::HitResult(Entity& entity, const Vec3& pos)
	: mType(ENTITY)
	, mFacing(0)
	, mEntity(&entity)
	, mIsHitLiquid(false)
	, mIndirectHit(false)
	, mPos(pos){
}


float HitResult::distanceTo(const Entity& otherEntity) const {
	const float xd = mPos.x - otherEntity.mPos.x;
	const float yd = mPos.y - otherEntity.mPos.y;
	const float zd = mPos.z - otherEntity.mPos.z;
	return xd * xd + yd * yd + zd * zd;
}

void HitResult::setOutOfRange() {
	if (mEntity != nullptr) {
		mType = ENTITY_OUT_OF_RANGE;
	}
	else {
		mType = NO_HIT;
	}
}
