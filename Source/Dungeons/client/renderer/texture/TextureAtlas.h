#pragma once

#include "common_header.h"

#include "client/renderer/texture/TextureAtlasItem.h"
#include "client/renderer/texture/TextureAtlasTile.h"
#include "legacy/Core/Resource/ResourceHelper.h"
//#include "AppPlatformListener.h"
#include "legacy/Core/Math/Color.h"
//#include "resources/ResourcePackManager.h"

class ResourcePack;

struct ParsedAtlasNodeElement {
	ResourceLocation location;
	Color overlay = Color::NIL;
	TextureUVCoordinateSet uv;
	float mipFadeAmount = 0.0f;
	Color mipFadeColor = Color::NIL;
};

struct ParsedAtlasNode {
	std::string name;
	bool quad = false;
	std::vector<ParsedAtlasNodeElement> elements;
};

namespace mce {
	class TextureGroup;
	class TexturePtr;
}

enum class TextureAtlasId {
	TerrainTexture
};

class TextureAtlas {
public:
	static const uint32_t MIN_TILE_SIDE;

	//TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures = nullptr);
	TextureAtlas();
	~TextureAtlas();
	const TextureAtlasItem& getTextureItem(const std::string& textureName) const;
	FVector2D getAtlasFaceSize() const;
	int getPadSize() const;

	void onAppResumed();

	void loadMetaFile(const ResourcePack&);

	void cleanTextureGroups();
	void redrawAtlas();
	void redrawAtlasD11(); //D11.PS - Temp diversion

	UTexture2D* atlasTexture = nullptr;
	UTexture2D* atlasTextureEmissive = nullptr;

	std::map<ResourceLocation, UTexture2D*> mTextureGroup;
	std::map<ResourceLocation, UTexture2D*> mTextureGroupEmissive;

#ifndef MCPE_HEADLESS
	const ParsedAtlasNode& getParsedNode(int index) const {
		return mAtlasNodesList[index];
	}
#endif

protected:
	//mce::TextureGroup* mTextureGroup;
	//ResourcePackManager& mResourcePackManager;

	std::unordered_map<std::string, TextureAtlasItem> mTextureItems;
	ResourceLocation mMetaLocation;
	TextureAtlasItem mMissingTextureItem;

	//intermediate metadata
#ifndef MCPE_HEADLESS
	//TODO do we actually need to keep all this metadata?
	std::vector<TextureAtlasTile> mTextureAtlasTiles;
	std::vector<ParsedAtlasNode> mAtlasNodesList;
#endif

	std::map<std::string, std::string> mResourceMap;

	std::string mResourceName;
	uint32_t mWidth, mHeight;

	int mPadSize = 0;
	int mNumMipLevels = 4;
	FVector2D mAtlasFaceSize;

	void _addElement(Json::Value texture, ParsedAtlasNode& newNode, int atIndex, float mipFadeAmount, const Color& mipFadeColor);
	void _readNode(Json::Value jsonNode, ParsedAtlasNode& node, bool isRefreshing);
	void _loadAtlasNodes(Json::Value root, std::vector<ParsedAtlasNode>& atlasNodes);
	void _addTile(std::vector<TextureAtlasTile> &tiles, TextureUVCoordinateSet* uvSet, bool quad, float fadePerMipAmount, const Color& fadeMipColor, ResourceLocation& location, const Color& overlayColor, const ResourcePack& resource);

	void _renderAtlasLevel(uint32_t mipID, std::vector<TextureAtlasTile>& tiles);
	void _renderAtlasLevelD11(uint32_t mipID, std::vector<TextureAtlasTile>& tiles, std::shared_ptr<uint8> atlasSrc);
	bool _tryAssignUVs(uint32_t width, uint32_t height, std::vector<TextureAtlasTile>& tiles);
};
