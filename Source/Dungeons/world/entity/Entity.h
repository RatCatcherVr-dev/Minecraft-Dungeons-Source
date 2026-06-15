/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/EntityTypes.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"
#include "world/level/ChunkPos.h"
#include "world/phys/AABB.h"
#include "world/Direction.h" 
#include "util/Random.h"
#include "EntityUniqueID.h"

class Level;

enum class EntityFlags : int {
	ONFIRE,
	SNEAKING,
	RIDING,
	SPRINTING,
	USINGITEM,
	INVISIBLE,
	TEMPTED,
	INLOVE,
	SADDLED,
	POWERED,
	IGNITED,
	BABY,
	CONVERTING,
	CRITICAL,
	CAN_SHOW_NAME,
	ALWAYS_SHOW_NAME,
	NOAI,
	SILENT,
	WALLCLIMBING,
	RESTING,
	SITTING,
	ANGRY,
	INTERESTED,
	CHARGED,
	TAMED,
	LEASHED,
	SHEARED,
	GLIDING,
	ELDER,
	MOVING,
	BREATHING,
	CHESTED,
	STACKABLE,
	SHOW_BOTTOM,
	STANDING,
	SHAKING,
	IDLING,
	COUNT
};

struct EntityDefinitionIdentifier;

class Entity {
public:
	enum class InitializationMethod : byte {
		INVALID = 0,
		LOADED,
		SPAWNED,
		BORN,
		TRANSFORMED,
		UPDATED,
		LEGACY
	};

	// Constructor for Player classes
	Entity(Level& level);
	virtual ~Entity();
public:
//	// Level
	bool mAdded = false;
	Level *mLevel = nullptr;

	static const std::string RIDING_TAG;
	static const int TOTAL_AIR_SUPPLY;

	mutable EntityUniqueID mUniqueID;

	// Placement
	Vec3 mPos = Vec3::ZERO;
	Vec3 mPosPrev = Vec3::ZERO;
	Vec3 mPosPrev2 = Vec3::ZERO;
	Vec3 mPosDelta = Vec3::ZERO;
	Vec2 mRot = Vec2::ZERO;
	Vec2 mRotPrev = Vec2::ZERO;
	Vec2 mRotRide = Vec2::ZERO;
	ChunkPos mChunkPos;
	BlockSource* mRegion = nullptr;

//	// Sync
	Vec3 mSentDelta = Vec3::ZERO;
	//TODO move this to the client side
	float mScale = 1.f;
	float mScalePrev = 0.f;

	// Physics
	bool mNoPhysics = false;
	bool mNoGravity = false;
	bool mOnGround = false;
	bool mWasOnGround = false;
	bool mHorizontalCollision = false;
	bool mVerticalCollision = false;
	bool mCollision = false;
	BlockID mInsideBlockId = BlockID::AIR;
	float mFallDistance = 0;
	
	// Visual
	bool mIgnoreLighting = false;

	//TODO move to EntityGraphics
	float mStepSoundVolume = 0.25f;
	float mStepSoundPitch = 1.f;
	AABB* mLastHitBB;
	AABB mBB;
	std::vector<AABB> mSubBBs;
	Vec2 mBBDim = { 0.6f, 1.8f }; // *** Def ***

	float mTerrainSurfaceOffset = 0; // *** Def ***
	float mHeightOffset = 0; // *** Def ***
	float mShadowOffset = 0; // to account for mismatches with the visual shape and the aabb (e.g. ghast tendrils)
	float mMaxAutoStep = 0.0f; // *** Def ***
	float mPushthrough = 0; // *** Def ***
	float mWalkDistPrev = 0;
	float mWalkDist = 0;
	float mMoveDist = 0;
	int mNextStep = 1;
	bool mMakeStepSound = true; // *** Def ***
	bool mIsStuckInWeb = false;
	bool mImmobile = false;
	bool mWasInWater = false;
	Vec2 mSlideOffset = Vec2::ZERO;

	// Time
	bool mFirstTick = true;
	int mTickCount = 0;
	int mInvulnerableTime = 0;

	// Health State
	bool mFallDamageImmune = true;
	bool mHurtMarked = false;
	bool mInvulnerable = false;

	// Fire State
	bool mFireImmune = false;
	bool mAlwaysFireImmune = false;

	int mOnFire = 0;
	bool mHurtWhenWet = false;

	bool mBlocksBuilding = false;
	bool mUsesOneWayCollision = false;
	bool mForcedLoading = false;
	bool mPrevPosRotSetThisTick = false;
	float mSoundVolume = 1.0f;

	//Util
	Random mRandom;
	
protected:
	bool mChanged = false;
	bool mRemoved = false;
	bool mGlobal = false;
	bool mAutonomous = false;
public:
	BlockSource& getRegion() const;

	const EntityUniqueID& getUniqueID() const;
	bool hasUniqueID() const;
	void setUniqueID(EntityUniqueID ID);
	void setRegion(BlockSource& region);

	bool operator==(Entity& rhs);

	virtual void remove();

	virtual void setPos(const Vec3& pos);
	virtual const Vec3& getPos() const;
	virtual const Vec3& getPosOld() const;
	virtual const Vec3 getPosExtrapolated(float factorAlpha) const;
	virtual const Vec3& getVelocity() const;

	virtual void setRot(const Vec2& rot);

	Vec2 getRotation() const;

	Vec3 getInterpolatedPosition(float a) const;
	Vec3 getInterpolatedPosition2(float a) const;
	Vec2 getInterpolatedRotation(float a) const;

	Vec3 getViewVector(float a) const;
	Vec2 getViewVector2(float a) const;


	virtual bool canFly() { return false; }

	void moveTo(const Vec3& pos, const Vec2& rot);

	virtual bool isInWater() const;

	virtual bool isSwimmer() { return false; }

	Vec3 getCenter(float a) const;

	float distanceTo(const Entity& e) const;
	float distanceTo(const Vec3& v) const;
	float distanceToSqr(const Vec3& v) const;
	float distanceToSqr(const Entity& e) const;
	float distanceSqrToBlockPosCenter(const BlockPos& incomingPosition) const;

	virtual void playerTouch(Player& player);

	virtual bool isImmobile() const;
	virtual bool isSilent();
	virtual bool isPickable();
	virtual bool isFishable() const;
	virtual bool isPushable() const;
	virtual bool isPushableByPiston() const;
	virtual bool isShootable();

	virtual bool isSneaking() const;
	virtual bool isAlive() const;
	virtual bool isOnFire() const;
	virtual bool isCreativeModeAllowed();
	virtual bool isSurfaceMob() const;

	virtual EntityType getEntityTypeId() const = 0;

	bool isRemoved() const;

	Level& getLevel();
	const Level& getLevel() const;

	virtual float getYHeadRot() const;

	virtual bool isCreative() const;
};
