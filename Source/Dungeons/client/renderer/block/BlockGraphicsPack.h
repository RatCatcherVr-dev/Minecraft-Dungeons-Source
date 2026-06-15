/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "BlockGraphics.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "legacy/Core/Math/Color.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "client/renderer/texture/TextureAtlasItem.h"
#include "lovika/LevelCommon.h"
#include "world/level/Rect2D.h"
#include "world/level/block/Block.h"
#include "world/phys/AABB.h"
#include "StrongObjectPtr.h"

class ResourcePack;
class BlockPos;
class BlockSource;
class TextureAtlas;
class TextureAtlasItem;

struct BlockUVMapping {
	const float TEXTURE_OFFSET = 0.0001f;
	void generateUV(const TextureAtlas& textureAtlas) {
		for (const auto& pair : textureAtlas.mTextureGroup) {
			if (pair.second) {
				std::vector<Rect2D> fullRect;
				fullRect.push_back(Rect2D(0, 0, pair.second->GetSizeX(), pair.second->GetSizeY()));
				mTessellatedTextures[pair.first] = fullRect;
			}
		}
	}
	std::map<ResourceLocation, std::vector<Rect2D>> mTessellatedTextures;
};

class BlockGraphicsPack {
public:
	static const int NUM_BLOCK_TYPES = 768;


	BlockGraphicsPack(const ResourcePack&, std::shared_ptr<TextureAtlas>);
	BlockGraphics* mBlocks[NUM_BLOCK_TYPES];
	std::vector<Unique<BlockGraphics>> mOwnedBlocks;
	std::unordered_map<std::string, BlockGraphics*> mBlockLookupMap;

	const BlockGraphics& get(BlockID) const;
	void setTextureAtlas(std::shared_ptr<TextureAtlas> terrainAtlas);
	void reloadBlockUVs(TextureAtlas& changed);
	void initBlocks(const ResourcePack&);
	void teardownBlocks();
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const;
	TextureUVCoordinateSet getTextureUVCoordinateSet(const std::string& name, int id = 0);
	const TextureAtlasItem& getTextureItem(const std::string& name) const;
	void CreateMaterials(AActor* parentActor, UMaterialInterface* opaque, UMaterialInterface* alphaTest,
	                     UMaterialInterface* alphaTestFoliage, UMaterialInterface* leaves);
	const BlockUVMapping& getBlockUVMapping() const;
	UMaterialInstanceDynamic* GenerateLeafMaterial(UMaterialInterface*, AActor* parentActor);

	TStrongObjectPtr<UMaterialInstanceDynamic> atlasMaterialInstanceOpaque;
	TStrongObjectPtr<UMaterialInstanceDynamic> atlasMaterialInstanceAlpha;
	TStrongObjectPtr<UMaterialInstanceDynamic> atlasMaterialInstanceAlphaFoilage;
	TStrongObjectPtr<UMaterialInstanceDynamic> atlasMaterialInstanceLeaves;
	std::shared_ptr<TextureAtlas> terrainTextureAtlas;

private:
	BlockUVMapping mBlockUVMapping;
	UMaterialInstanceDynamic* createTextureAtlasDynamicMaterialInstance(UMaterialInterface*,
	                                                                    AActor* parentActor) const;
	BlockGraphics& registerBlockGraphics(const std::vector<Json::Value>& cascadedBlockDataJsons,
	                                     const std::string& blockName);
};

class BlockGraphicsManager {
public:
	~BlockGraphicsManager();
	BlockGraphicsPack& add(const ResourcePack&);
	BlockGraphicsPack& get(const ResourcePack&) const;
	BlockGraphicsPack& getLatest() const;
	void teardown();

private:
	std::map<FString, std::unique_ptr<BlockGraphicsPack>> mBlockGraphics;
	BlockGraphicsPack* mSelected = nullptr;
};

namespace BlockGraphicsHelper {
	const BlockGraphics& getBlock(UWorld*, BlockPos, BlockID);
	const BlockGraphics& getBlock(const BlockGraphicsPack&, BlockID);
	const BlockGraphicsPack& getBlockGraphicsPack(UWorld*, BlockPos);
	void CreateInstance(UWorld*);
	void RemoveInstance(UWorld*);
	BlockGraphicsManager& getInstance(UWorld*);
}
