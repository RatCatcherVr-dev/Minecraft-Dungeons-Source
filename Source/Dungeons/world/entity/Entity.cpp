/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/Entity.h"

#include "world/level/Level.h"
#include "world/level/BlockSource.h"

const int Entity::TOTAL_AIR_SUPPLY = 15 * SharedConstants::TicksPerSecond;
const std::string Entity::RIDING_TAG = "Riding";

Entity::Entity(Level& level)
	: mRegion(nullptr)
	, mLevel(&level)
{

	DEBUG_ASSERT_MAIN_THREAD;

	mPosDelta = Vec3::ZERO;
}

Entity::~Entity() {
}

bool Entity::hasUniqueID() const {
	return (bool)mUniqueID;
}

const EntityUniqueID& Entity::getUniqueID() const {
	//HACK the unique ID should be assigned non-lazily!
	if (!mUniqueID) {
		mUniqueID = mLevel->getNewUniqueID();
	}

	return mUniqueID;
}

void Entity::setUniqueID(EntityUniqueID ID) {
	DEBUG_ASSERT(ID, "Invalid ID");
	DEBUG_ASSERT(!mUniqueID, "This entity already has a unique ID you villain");

	//TODO HACK put a breakpoint here on a client to trigger spawn in the air!!!
	mUniqueID = ID;
}

bool Entity::isInWater() const {
	return mWasInWater;
}

void Entity::setPos(const Vec3 &pos) {
	mPos = pos;
	float w = mBBDim.x / 2;
	float h = mBBDim.y;
	mBB.set(pos.x - w, pos.y - mHeightOffset + mSlideOffset.y, pos.z - w, pos.x + w, pos.y - mHeightOffset + mSlideOffset.y + h, pos.z + w);
}

const Vec3& Entity::getPos() const {
	return mPos;
}

const Vec3& Entity::getPosOld() const {
	return mPosPrev;
}

const Vec3 Entity::getPosExtrapolated(float factorAlpha) const {
	//TODO this duplicates getInterpolatedPosition
	const Vec3 ret = getPosOld() + (getPos() - getPosOld()) * factorAlpha;
	return ret;
}

const Vec3& Entity::getVelocity() const {
	return mPosDelta;
}

Vec3 Entity::getCenter(float a) const {
	Vec3 curPos = getInterpolatedPosition(a);

	curPos.y += mBB.getBounds().y * 0.66f - mHeightOffset;
	return curPos;
}

Vec3 Entity::getViewVector(float a) const {
	Vec2 rot = getInterpolatedRotation(a);

	float yCos = Math::cos(-rot.y * Math::DEGRAD - PI);
	float ySin = Math::sin(-rot.y * Math::DEGRAD - PI);
	float xCos = -Math::cos(-rot.x * Math::DEGRAD);
	float xSin = Math::sin(-rot.x * Math::DEGRAD);

	return Vec3(ySin * xCos, xSin, yCos * xCos);
}

Vec2 Entity::getViewVector2(float a) const {
	Vec2 rot = getInterpolatedRotation(a);
	return Vec2(-Math::cos(rot.y * Math::DEGRAD - PI / 2), -Math::sin(rot.y * Math::DEGRAD - PI / 2));
}

bool Entity::operator==(Entity& rhs) {
	return mUniqueID == rhs.mUniqueID;
}

void Entity::remove() {
	mRemoved = true;
}

void _rotationWrapWithInterpolation(float& rot, float& oRot) {
	while (rot > 360.f) {
		rot -= 360.f;
		oRot -= 360.f;
	}

	while (rot < -360.f) {
		rot += 360.f;
		oRot += 360.f;
	}
}

void Entity::setRot(const Vec2& rot) {
	mRot.y = rot.y;
	mRot.x = rot.x;

	_rotationWrapWithInterpolation(mRot.y, mRotPrev.y);
	_rotationWrapWithInterpolation(mRot.x, mRotPrev.x);
}

void Entity::moveTo(const Vec3& pos, const Vec2& rot) {
	setRot(rot);
	setPos(pos + Vec3(0, mHeightOffset, 0));
}

float Entity::distanceTo(const Entity& e) const {
	float xd = (mPos.x - e.mPos.x);
	float yd = (mPos.y - e.mPos.y);
	float zd = (mPos.z - e.mPos.z);
	return sqrt(xd * xd + yd * yd + zd * zd);
}

float Entity::distanceTo(const Vec3& pos) const {
	float xd = (mPos.x - pos.x);
	float yd = (mPos.y - pos.y);
	float zd = (mPos.z - pos.z);
	return sqrt(xd * xd + yd * yd + zd * zd);
}

float Entity::distanceToSqr(const Vec3& pos) const {
	float xd = (mPos.x - pos.x);
	float yd = (mPos.y - pos.y);
	float zd = (mPos.z - pos.z);
	return xd * xd + yd * yd + zd * zd;
}

float Entity::distanceToSqr(const Entity& e) const {
	float xd = mPos.x - e.mPos.x;
	float yd = mPos.y - e.mPos.y;
	float zd = mPos.z - e.mPos.z;
	return xd * xd + yd * yd + zd * zd;
}

float Entity::distanceSqrToBlockPosCenter(const BlockPos& pos) const {
	return pos.distSqrToCenter(mPos.x, mPos.y, mPos.z);
}

void Entity::playerTouch(Player& player) {
}

BlockSource& Entity::getRegion() const {
	return *mRegion;
}

bool Entity::isPickable() {
	return false;
}

bool Entity::isFishable() const {
	return true;
}

bool Entity::isPushable() const {
	return false;
}

bool Entity::isPushableByPiston() const {
	return isPushable();
}

bool Entity::isShootable() {
	return false;
}

bool Entity::isCreativeModeAllowed() {
	return false;
}

bool Entity::isSurfaceMob() const {
	return true;
}

bool Entity::isAlive() const {
	return !mRemoved;
}

bool Entity::isSneaking() const {
	return false;
}

bool Entity::isOnFire() const {
	return mOnFire > 0;
}

bool Entity::isImmobile() const {
	//return mImmobile || getStatusFlag(EntityFlags::NOAI);
	return mImmobile;
}

bool Entity::isSilent() {
	//return getStatusFlag(EntityFlags::SILENT);
	return true;
}

bool Entity::isRemoved() const {
	return mRemoved;
}

void Entity::setRegion(BlockSource& source) {
	mRegion = &source;
}

Vec3 Entity::getInterpolatedPosition(float a) const {
	return Vec3(
		mPosPrev2.x + (mPos.x - mPosPrev2.x) * a,
		mPosPrev2.y + (mPos.y - mPosPrev2.y) * a,
		mPosPrev2.z + (mPos.z - mPosPrev2.z) * a);
}

Vec3 Entity::getInterpolatedPosition2(float a) const {
	return Vec3(
		mPosPrev.x + (mPos.x - mPosPrev.x) * a,
		mPosPrev.y + (mPos.y - mPosPrev.y) * a,
		mPosPrev.z + (mPos.z - mPosPrev.z) * a);
}

Vec2 Entity::getRotation() const {
	return Vec2(mRot.x, mRot.y);
}

Vec2 Entity::getInterpolatedRotation(float a) const {
	auto rot = Vec2(
		mRotPrev.x + (mRot.x - mRotPrev.x) * a,
		mRotPrev.y + (mRot.y - mRotPrev.y) * a);

	return rot;
}

Level& Entity::getLevel() {
	DEBUG_ASSERT(mLevel, "Entity was not placed in a level");
	return *mLevel;
}

const Level& Entity::getLevel() const {
	DEBUG_ASSERT(mLevel, "Entity was not placed in a level");
	return *mLevel;
}

float Entity::getYHeadRot() const {
	return 0.f;
}

bool Entity::isCreative() const {
	return false;
}
