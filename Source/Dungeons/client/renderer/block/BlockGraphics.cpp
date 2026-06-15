/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/block/BlockGraphics.h"
#include "BlockGraphicsPack.h"
#include "world/Facing.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/Block.h"
#include "world/level/block/BlockInclude.h"
#include "world/level/BlockSource.h"
#include "world/phys/Vec3.h"
// #include "platform/AppPlatform.h"
#include "client/resource/Resource.h"
#include "util/EnumUtil.h"
#include <VoiceConfig.h>

#include "game/Game.h"

//------------------------------------------------------------------------------
BlockSoundType BlockSoundTypeFromString(const std::string& type) {
	static std::unordered_map<std::string, BlockSoundType> mapping = {
		{ "normal",		BlockSoundType::Normal },
		{ "gravel",		BlockSoundType::Gravel },
		{ "wood",		BlockSoundType::Wood },
		{ "grass",		BlockSoundType::Grass },
		{ "metal",		BlockSoundType::Metal },
		{ "stone",		BlockSoundType::Stone },
		{ "cloth",		BlockSoundType::Cloth },
		{ "glass",		BlockSoundType::Glass },
		{ "sand",		BlockSoundType::Sand },
		{ "snow",		BlockSoundType::Snow },
		{ "ladder",		BlockSoundType::Ladder },
		{ "anvil",		BlockSoundType::Anvil },
		{ "slime",		BlockSoundType::Slime },
		{ "silent",		BlockSoundType::Silent },
		{ "default",	BlockSoundType::Default }
	};
	auto res = mapping.find(Util::toLower(type));
	if (res != mapping.end()) {
		return res->second;
	}
	else {
		return BlockSoundType::Undefined;
	}
}
std::string BlockSoundTypeToString(BlockSoundType type) {
	static std::map<BlockSoundType, std::string> typeToString = {
		{ BlockSoundType::Normal,	"normal" },
		{ BlockSoundType::Gravel,	"gravel" },
		{ BlockSoundType::Wood,		"wood" },
		{ BlockSoundType::Grass,	"grass" },
		{ BlockSoundType::Metal,	"metal" },
		{ BlockSoundType::Stone,	"stone" },
		{ BlockSoundType::Cloth,	"cloth" },
		{ BlockSoundType::Glass,	"glass" },
		{ BlockSoundType::Sand,		"sand" },
		{ BlockSoundType::Snow,		"snow" },
		{ BlockSoundType::Ladder,	"ladder" },
		{ BlockSoundType::Anvil,	"anvil" },
		{ BlockSoundType::Slime,	"slime" },
		{ BlockSoundType::Silent,	"silent" },
		{ BlockSoundType::Default,	"default" },
		{ BlockSoundType::Undefined,"undefined" }
	};
	return typeToString[type];
}

const float BlockGraphics::SIZE_OFFSET = 0.0001f;

bool BlockGraphics::setTextures(const Json::Value& textureData){
	if (textureData.isNull())
		return true;

	if (textureData.isString()) {
		setTextureItem(textureData.asString());
		return true;
	}

	if (textureData.isObject()) {
		auto textureNames = textureData.getMemberNames();
		int textureNum = textureNames.size();
		if ((textureNum == 3) && (textureData["up"].isString() && textureData["down"].isString() && textureData["side"].isString())) {
			setTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["side"].asString());
			return true;
		}
		if ((textureNum == 6) &&
			(textureData["up"].isString() && textureData["down"].isString() && textureData["north"].isString() &&
				textureData["south"].isString() && textureData["west"].isString() && textureData["east"].isString())) {
			setTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["north"].asString(),
				textureData["south"].asString(), textureData["west"].asString(), textureData["east"].asString());
			return true;
		}
	}
	return false;
}

bool BlockGraphics::setCarriedTextures(const Json::Value& textureData)
{
	if (textureData.isNull())
		return true;

	if (textureData.isString()) {
		setCarriedTextureItem(textureData.asString());
		return true;
	}

	if (textureData.isObject()) {
		auto textureNames = textureData.getMemberNames();
		int textureNum = textureNames.size();
		if ((textureNum == 3) && (textureData["up"].isString() && textureData["down"].isString() && textureData["side"].isString())) {
			setCarriedTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["side"].asString());
			return true;
		}
		if ((textureNum == 6) &&
			(textureData["up"].isString() && textureData["down"].isString() && textureData["north"].isString() &&
				textureData["south"].isString() && textureData["west"].isString() && textureData["east"].isString())) {
			setCarriedTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["north"].asString(),
				textureData["south"].asString(), textureData["west"].asString(), textureData["east"].asString());
			return true;
		}
	}
	return false;
}


bool BlockGraphics::setTextureIsotropic(const Json::Value& isotropicData)
{
	if (isotropicData.isNull())
		return true;

	if (isotropicData.isBool()) {
		if (isotropicData.asBool()) {
			setAllFacesIsotropic();
		}
		return true;
	}

	if (isotropicData.isObject()) {
		static std::map<std::string, int> faceIsotropicMap_3 = {
			{ "up",			1 << Facing::UP },
			{ "down",		1 << Facing::DOWN },
			{ "side",		1 << Facing::NORTH | 1 << Facing::SOUTH | 1 << Facing::WEST | 1 << Facing::EAST }
		};
		static std::map<std::string, int> faceIsotropicMap_6 = {
			{ "up",			1 << Facing::UP },
			{ "down",		1 << Facing::DOWN },
			{ "north",		1 << Facing::NORTH },
			{ "south",		1 << Facing::SOUTH },
			{ "west",		1 << Facing::WEST },
			{ "east",		1 << Facing::EAST }
		};

		int isotropicValue = 0x00;
		int count = isotropicData.getMemberNames().size();

		for (auto& a : isotropicData["side"].isNull() ? faceIsotropicMap_6 : faceIsotropicMap_3) {
			if (isotropicData[a.first].isBool()) {
				count--;
				if (isotropicData[a.first].asBool() == true) {
					isotropicValue |= a.second;
				}
			}
		}

		setTextureIsotropic(isotropicValue);
		return count == 0;
	}
	return false;
}

bool BlockGraphics::setBlockShape(const Json::Value& blockShapeData) {
	static const std::map<std::string, BlockShape> nameToBlockShape = {
		{ "invisible",			BlockShape::INVISIBLE },
		{ "block",				BlockShape::BLOCK },
		{ "cross_texture",		BlockShape::CROSS_TEXTURE },
		{ "torch",				BlockShape::TORCH },
		{ "fire",				BlockShape::FIRE },
		{ "water",				BlockShape::WATER },
		{ "red_dust",			BlockShape::RED_DUST },
		{ "rows",				BlockShape::ROWS },
		{ "door",				BlockShape::DOOR },
		{ "ladder",				BlockShape::LADDER },
		{ "rail",				BlockShape::RAIL },
		{ "stairs",				BlockShape::STAIRS },
		{ "fence",				BlockShape::FENCE },
		{ "lever",				BlockShape::LEVER },
		{ "cactus",				BlockShape::CACTUS },
		{ "bed",				BlockShape::BED },
		{ "diode",				BlockShape::DIODE },
		{ "iron_fence",			BlockShape::IRON_FENCE },
		{ "stem",				BlockShape::STEM },
		{ "vine",				BlockShape::VINE },
		{ "fence_gate",			BlockShape::FENCE_GATE },
		{ "chest",				BlockShape::CHEST },
		{ "lilypad",			BlockShape::LILYPAD },
		{ "brewing_stand",		BlockShape::BREWING_STAND },
		{ "portal_frame",		BlockShape::PORTAL_FRAME },
		{ "cocoa",				BlockShape::COCOA },
		{ "tree",				BlockShape::TREE },
		{ "wall",				BlockShape::WALL },
		{ "double_plant",		BlockShape::DOUBLE_PLANT },
		{ "flower_pot",			BlockShape::FLOWER_POT },
		{ "anvil",				BlockShape::ANVIL },
		{ "cross_texture_poly",	BlockShape::CROSS_TEXTURE_POLY },
		{ "double_plant_poly",	BlockShape::DOUBLE_PLANT_POLY },
		{ "block_half",			BlockShape::BLOCK_HALF },
		{ "top_snow",			BlockShape::TOP_SNOW },
		{ "tripwire",			BlockShape::TRIPWIRE },
		{ "tripwire_hook",		BlockShape::TRIPWIRE_HOOK },
		{ "cauldron",			BlockShape::CAULDRON },
		{ "repeater",			BlockShape::REPEATER },
		{ "comparator",			BlockShape::COMPARATOR },
		{ "hopper",				BlockShape::HOPPER },
		{ "slime_block",		BlockShape::SLIME_BLOCK },
		{ "piston",				BlockShape::PISTON },
		{ "beacon",				BlockShape::BEACON},
		{ "chorus_plant",		BlockShape::CHORUS_PLANT },
		{ "chorus_flower",		BlockShape::CHORUS_FLOWER },
		{ "dragon_egg",			BlockShape::DRAGON_EGG },
		{ "end_portal",			BlockShape::END_PORTAL },
		{ "end_rod",			BlockShape::END_ROD },
		{ "structure_void",		BlockShape::STRUCTURE_VOID },
		{ "leaves",             BlockShape::BLOCK },
		{ "leaves2",            BlockShape::BLOCK },
		{ "void",				BlockShape::VOID_BLOCK },

	};

	if (blockShapeData.isNull()) {
		return true;
	}

	if (blockShapeData.isString()) {
		auto elem = nameToBlockShape.find(blockShapeData.asString());
		if (elem != nameToBlockShape.end()) {
			setBlockShape(elem->second);
			return true;
		}
	}
	return false;
}

bool BlockGraphics::setBrightnessGamma(const Json::Value& data) {
	if (data.isNull())
		return true;
	if (data.isNumeric()) {
		setBrightnessGamma(data.asFloat());
		return true;

	}
	return false;
}

bool BlockGraphics::setSoundType(const Json::Value& data) {
	if (!data.isNull()) {
		BlockSoundType type = BlockSoundTypeFromString(data.asString("normal"));
		if (type != BlockSoundType::Undefined) {
			setSoundType(type);
		}
	}
	return true;
}

bool BlockGraphics::setMaterialType(const Json::Value& data) {
	if (!data.isNull()) {
		if (auto material = EnumValueFromString(EMaterialTypeEnum, data.asString().c_str())) {
			setMaterialType(material.GetValue());
		}
	}
	return true;
}

const Block* BlockGraphics::getBlock() const {
	return mBlock;
}


BlockGraphics::BlockGraphics(const BlockGraphicsPack& parent, const std::string& nameId)
	: mID(0)
	, mSoundType(BlockSoundType::Normal)
	, mBlockShape(BlockShape::BLOCK)
	, mParent(parent) {

	mBlock = Block::lookupByName(nameId);
	DEBUG_ASSERT(mBlock != nullptr, "Invalid Block Graphics name");
	mID = mBlock->mID;
	//DEBUG_ASSERT(!BlockGraphics::mBlocks[mID], "Slot is already occupied");
	mBrightnessGamma = mBlock->getBrightnessGamma();
}

BlockGraphics::~BlockGraphics() {
}

Color BlockGraphics::getMapColor(const FullBlock& block) const {
	return mMapColor;
}

Color BlockGraphics::getMapColor() const {
	return mMapColor;
}

void BlockGraphics::animateTick(BlockSource& region, const BlockPos& pos, Random& random) {
}

const TextureUVCoordinateSet& BlockGraphics::getTexture(FacingID face, int variant) const {
	if (variant < 0 || variant >= (int)mUVItem[face].uvCount()) {
		variant = 0;
	}
	return mUVItem[face][variant];
}

const TextureAtlasItem& BlockGraphics::getAtlasItem(FacingID face) const {
	return mUVItem[face];
}

const TextureUVCoordinateSet& BlockGraphics::getTexture(FacingID face) const {
	return mUVItem[face][0];
}

const TextureUVCoordinateSet& BlockGraphics::getCarriedTexture(FacingID face, int variant) const {
	if (mUVCarried[face].uvCount() > 0) {
		if (variant < 0 || variant >= (int)mUVCarried[face].uvCount()) {
			variant = 0;
		}
		return mUVCarried[face][variant];
	}
	else {
		return getTexture(face, variant);
	}
}

bool BlockGraphics::isTextureIsotropic(FacingID face) const {
	return ((mFaceTextureIsotropic >> face) & 0x1) == 1;
}

Vec3 BlockGraphics::randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const {
	seed = 0;
	return randomlyModifyPosition(pos);
}

Vec3 BlockGraphics::randomlyModifyPosition(const BlockPos& pos) const {
	return pos;
}

int BlockGraphics::getColor(int auxData) const {
	return 0xffffffff;
}

int BlockGraphics::getColor(BlockSource& region, const BlockPos& pos) const {
	return 0xffffffff;
}

int BlockGraphics::getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const {
	return getColor(region, pos);
}

BlockGraphics& BlockGraphics::setSoundType(BlockSoundType type) {
	mSoundType = type;
	return *this;
}

BlockGraphics& BlockGraphics::setMaterialType(EMaterialTypeEnum material) {
	mMaterial = material;
	return *this;
}

BlockGraphics& BlockGraphics::setMapColor(const Color& color) {
	mMapColor = color;
	return *this;
}

BlockGraphics& BlockGraphics::setTextureItem(const std::string& name) {
	return setTextureItem(name, name, name, name, name, name);
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& name) {
	return setCarriedTextureItem(name, name, name);
}


BlockGraphics& BlockGraphics::setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide) {
	return setTextureItem(nameUp, nameDown, nameSide, nameSide, nameSide, nameSide);
}

BlockGraphics& BlockGraphics::setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast) {
	mUVItem[Facing::UP] = mParent.getTextureItem(mUVItemName[Facing::UP] = nameUp);
	mUVItem[Facing::DOWN] = mParent.getTextureItem(mUVItemName[Facing::DOWN] = nameDown);
	mUVItem[Facing::NORTH] = mParent.getTextureItem(mUVItemName[Facing::NORTH] = nameNorth);
	mUVItem[Facing::SOUTH] = mParent.getTextureItem(mUVItemName[Facing::SOUTH] = nameSouth);
	mUVItem[Facing::WEST] = mParent.getTextureItem(mUVItemName[Facing::WEST] = nameWest);
	mUVItem[Facing::EAST] = mParent.getTextureItem(mUVItemName[Facing::EAST] = nameEast);
	return *this;
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide) {
	return setCarriedTextureItem(nameUp, nameDown, nameSide, nameSide, nameSide, nameSide);
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast) {
	mUVCarried[Facing::UP] = mParent.getTextureItem(mUVCarriedName[Facing::UP] = nameUp);
	mUVCarried[Facing::DOWN] = mParent.getTextureItem(mUVCarriedName[Facing::DOWN] = nameDown);
	mUVCarried[Facing::NORTH] = mParent.getTextureItem(mUVCarriedName[Facing::NORTH] = nameNorth);
	mUVCarried[Facing::SOUTH] = mParent.getTextureItem(mUVCarriedName[Facing::SOUTH] = nameSouth);
	mUVCarried[Facing::WEST] = mParent.getTextureItem(mUVCarriedName[Facing::WEST] = nameWest);
	mUVCarried[Facing::EAST] = mParent.getTextureItem(mUVCarriedName[Facing::EAST] = nameEast);
	return *this;
}
void BlockGraphics::setVisualShape(const Vec3& min, const Vec3& max) {
	mVisualShape.set(min, max);
}

void BlockGraphics::setVisualShape(const AABB& shape) {
	mVisualShape.set(shape);
}

void BlockGraphics::onGraphicsModeChanged(bool fancy, bool preferPolyTessellation) {
	mFancy = fancy;

	if (mID == Block::mTallgrass->mID) {
		if (preferPolyTessellation) {
			mBlockShape = BlockShape::CROSS_TEXTURE_POLY;
		}
		else {
			mBlockShape = BlockShape::CROSS_TEXTURE;
		}
	}
	else if (mID == Block::mDoublePlant->mID) {
		if (preferPolyTessellation) {
			mBlockShape = BlockShape::DOUBLE_PLANT_POLY;
		}
		else {
			mBlockShape = BlockShape::DOUBLE_PLANT;
		}
	}

}

BlockShape BlockGraphics::getBlockShape() const {
	return mBlockShape;
}

BlockGraphics& BlockGraphics::setBlockShape(BlockShape shape) {
	mBlockShape = shape;
	return *this;
}

BlockGraphics& BlockGraphics::setAllFacesIsotropic() {
	mFaceTextureIsotropic = 0xff;
	return *this;
}


int BlockGraphics::getIconYOffset() const {
	return 0;
}

bool BlockGraphics::isAlphaTested() {
	auto rl = mRenderLayer;
	return rl == RENDERLAYER_ALPHATEST || rl == RENDERLAYER_ALPHATEST_SINGLE_SIDE || (mFancy && rl == RENDERLAYER_OPTIONAL_ALPHATEST);
}

BlockRenderLayer BlockGraphics::getRenderLayer(BlockSource& region, const BlockPos& pos) const {
	return static_cast<BlockRenderLayer>(getBlock()->getRenderLayer());
	// REFACTOR TODO: Replace with line below when BlockGraphics BlockRenderLayer handling is finished
	// return mRenderLayer;
}

BlockRenderLayer BlockGraphics::getRenderLayer() const {
	return static_cast<BlockRenderLayer>(getBlock()->getRenderLayer());
	// REFACTOR TODO: Replace with line below when BlockGraphics BlockRenderLayer handling is finished
	// return mRenderLayer;
}

int BlockGraphics::getExtraRenderLayers() {
	return 0;
}

bool BlockGraphics::isFullAndOpaque() const {
	if ((getBlockShape() == BlockShape::BLOCK || getBlockShape() == BlockShape::TREE) && (getRenderLayer() == BlockRenderLayer::RENDERLAYER_OPAQUE || getRenderLayer() == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE || getRenderLayer() == BlockRenderLayer::RENDERLAYER_OPAQUE_FILL)) {
		//use the visual shape to make sure that this block is actually full
		//TODO do *not* use getBlockShape() for this. Only the actual Visual Shape should decide this.
		AABB bbb;
		bbb = getVisualShape(0, bbb);
		return (bbb.getBounds() == Vec3::ONE);
	}

	return false;
}

const AABB& BlockGraphics::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, false);
}

const AABB& BlockGraphics::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping	/* = false*/) const {
	return mVisualShape;
}

unsigned int BlockGraphics::getFaceTextureIsotropic() {
	return mFaceTextureIsotropic;
}

BlockGraphics& BlockGraphics::setTextureIsotropic(unsigned int faces) {
	mFaceTextureIsotropic = faces;
	return *this;
}

BlockSoundType  BlockGraphics::getSoundType() const {
	return mSoundType;
}

EMaterialTypeEnum BlockGraphics::getMaterialType(/*DataID*/) const {
	return mMaterial;
}

