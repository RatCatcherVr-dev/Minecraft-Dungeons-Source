/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

//#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "legacy/Core/Math/Color.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "client/renderer/texture/TextureAtlasItem.h"
#include "lovika/LevelCommon.h"
#include "world/level/block/Block.h"
#include "world/phys/AABB.h"

class BlockPos;
class Entity;
class Mob;
class Player;
class BlockSource;
class Material;
class Random;
class ItemInstance;
class TextureAtlas;
class Material;
class Mob;
class TextureAtlasItem;
class BlockGraphicsPack;

enum class EntityType : unsigned int;
enum class CreativeItemCategory : int;
enum class BlockShape : int {

	INVISIBLE = -1,
	BLOCK = 0,
	CROSS_TEXTURE = 1,
	TORCH = 2,
	FIRE = 3,
	WATER = 4,
	RED_DUST = 5,
	ROWS = 6,
	DOOR = 7,
	LADDER = 8,
	RAIL = 9,
	STAIRS = 10,
	FENCE = 11,
	LEVER = 12,
	CACTUS = 13,
	BED = 14,
	DIODE = 15,
	IRON_FENCE = 18,
	STEM = 19,
	VINE = 20,
	FENCE_GATE = 21,
	CHEST = 22,
	LILYPAD = 23,
	BREWING_STAND = 25,
	PORTAL_FRAME = 26,
	COCOA = 28,
	TREE = 31,
	WALL = 32,
	DOUBLE_PLANT = 40,
	FLOWER_POT = 42,
	ANVIL = 43,
	DRAGON_EGG = 44,
	STRUCTURE_VOID = 48,
	CROSS_TEXTURE_POLY = 65,
	DOUBLE_PLANT_POLY = 66,
	BLOCK_HALF = 67,
	TOP_SNOW = 68,
	TRIPWIRE = 69,
	TRIPWIRE_HOOK = 70,
	CAULDRON = 71,
	REPEATER = 72,
	COMPARATOR = 73,
	HOPPER = 74,
	SLIME_BLOCK = 75,
	PISTON = 76,
	BEACON = 77,
	CHORUS_PLANT = 78,
	CHORUS_FLOWER = 79,
	END_PORTAL = 80,
	END_ROD = 81,
	END_GATEWAY = 82,
	SKULL = 83,
	LEAVES = 84,
	VOID_BLOCK = 85
};

class Block;

enum class BlockSoundType : int {
	Normal,
	Gravel,
	Wood,
	Grass,
	Metal,
	Stone,
	Cloth,
	Glass,
	Sand,
	Snow,
	Ladder,
	Anvil,
	Slime,
	Silent,
	Default,
	Undefined
};

BlockSoundType BlockSoundTypeFromString(const std::string& str);
std::string BlockSoundTypeToString(BlockSoundType type);

// @Note: Got a memory leak in initBlocks? You probably didn't call
//        Block::init after constructing the block!
class BlockGraphics {
public:

	static const float SIZE_OFFSET;

	BlockID mID;

	void setTextureAtlas(std::shared_ptr<TextureAtlas> terrainAtlas);

	bool setTextures(const Json::Value& textureData);
	bool setCarriedTextures(const Json::Value& textureData);
	bool setTextureIsotropic(const Json::Value& isotropicData);
	bool setBlockShape(const Json::Value& blockShapeData);
	bool setBrightnessGamma(const Json::Value& data);
	bool setSoundType(const Json::Value& data);
	bool setMaterialType(const Json::Value& data);

	BlockGraphics(const BlockGraphicsPack& parent, const std::string& nameId);
	virtual ~BlockGraphics();

	//void init(const Block* block); // Not gonna remove this since I kind of want a function like it
	const Block* getBlock() const;

	BlockID getID() const { return mID; }
	BlockShape getBlockShape() const;
	BlockGraphics& setBlockShape(BlockShape shape);


	const TextureAtlasItem& getAtlasItem(FacingID face) const;
	const TextureUVCoordinateSet& getTexture(FacingID face) const;
	const TextureUVCoordinateSet& getTexture(FacingID face, int variant) const;

	virtual int getIconYOffset() const;

	BlockSoundType getSoundType() const;
	EMaterialTypeEnum getMaterialType(/*DataID: add when needed*/) const;

	BlockRenderLayer getRenderLayer() const;

	virtual int getColor(int auxData) const;
	virtual int getColor(BlockSource& region, const BlockPos& pos) const;
	virtual int getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const;

	Color getMapColor(const FullBlock& block) const;
	Color getMapColor() const;

	virtual void onGraphicsModeChanged(bool fancy, bool preferPolyTessellation);
	virtual BlockRenderLayer getRenderLayer(BlockSource& region, const BlockPos& pos) const;
	virtual int getExtraRenderLayers();
	bool isAlphaTested();
	unsigned int getFaceTextureIsotropic();
	BlockGraphics& setTextureIsotropic(unsigned int faces);
	bool isTextureIsotropic(FacingID face) const;
	float getBrightnessGamma() const {
		return mBrightnessGamma;
	}

	BlockGraphics& setBrightnessGamma(float g) {
		mBrightnessGamma = g;
		return *this;
	}
	BlockGraphics& setMaterialType(EMaterialTypeEnum);

	BlockGraphics& setAllFacesIsotropic();

	// Whether a block is full-sized and opaque (i.e. if it covers the entire voxel/cube when rendered)
	bool isFullAndOpaque() const;

	// Visual Shape
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const;

	bool isTextureAnimated() const {
		return mAnimatedTexture;
	}
	virtual const TextureUVCoordinateSet& getCarriedTexture(FacingID face, int variant) const;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random);

	virtual Vec3 randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const;
	virtual Vec3 randomlyModifyPosition(const BlockPos& pos) const;

	BlockGraphics& setMapColor(const Color& color);
	virtual void setVisualShape(const AABB& shape);
	virtual void setVisualShape(const Vec3& min, const Vec3& max);

	BlockGraphics& setTextureItem(const std::string& name);
	BlockGraphics& setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide);
	BlockGraphics& setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast);
	BlockGraphics& setCarriedTextureItem(const std::string& name);
	BlockGraphics& setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide);
	BlockGraphics& setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast);
	BlockGraphics& setSoundType(BlockSoundType);

	TextureAtlasItem mUVItem[6];
	TextureAtlasItem mUVCarried[6];

	std::string mUVItemName[6], mUVCarriedName[6];
protected:
	static BlockGraphics* lookupByName(const std::string& name, bool caseInsensitive);

	const Block* mBlock = nullptr;
	unsigned int mFaceTextureIsotropic = 0;


	BlockRenderLayer mRenderLayer = RENDERLAYER_OPAQUE;
	BlockShape mBlockShape = BlockShape::BLOCK;
	bool mAnimatedTexture = false;
	float mBrightnessGamma = 1.f;

	Color mMapColor;
	bool mFancy;

private:
	BlockSoundType mSoundType;
	EMaterialTypeEnum mMaterial;
	// The shape of the object
	// (this can be overridden in block renderer, just want a default that makes sense)
	// Defaults to 0 0 0, 1 1 1
	AABB mVisualShape;
	const BlockGraphicsPack& mParent;
};

