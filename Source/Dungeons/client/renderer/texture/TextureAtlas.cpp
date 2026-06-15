#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlas.h"
//#include "platform/AppPlatform.h"
#include "legacy/Core/Resource/ResourceHelper.h"

#include "client/renderer/block/BlockGraphics.h"
#include "client/resource/Resource.h"
#include "util/JSONUtils.h"

#if !defined(MCPE_HEADLESS)
#include "client/renderer/renderer/Tessellator.h"
//#include "Renderer/HAL/Enums/RenderFeature.h"
//#include "Renderer/HAL/Enums/BindFlagsBit.h"
//#include "Renderer/HAL/Interface/RenderContextImmediate.h"
//#include "Renderer/HAL/Interface/RenderContext.h"
//#include "Renderer/HAL/Interface/RenderDevice.h"
//#include "Renderer/TextureGroup.h"
#endif

//#include <glm/gtc/type_precision.hpp>
//#include <glm/core/func_common.hpp>
//#include <glm/gtx/fast_square_root.hpp>

const uint32_t TextureAtlas::MIN_TILE_SIDE = 16;

TextureAtlas::~TextureAtlas() {

}

// #ifndef MCPE_HEADLESS

//TextureAtlas::TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures)
TextureAtlas::TextureAtlas()
	: mMissingTextureItem("", 255) // HACK: 255 different missing textures so objects can depend on specific ids inside it
	//, mMetaLocation(metaFile)
	//, mTextureGroup(textures)
	//, mResourcePackManager(resourcePackManager)
{

	//the Atlas needs to have a low priority to ensure that it gets the event after the materials are reloaded
	//initListener(10.f);
}

template<typename T>
T& select_min(T& A, T& B) {
	return A > B ? B : A;
}

bool TextureAtlas::_tryAssignUVs(uint32_t width, uint32_t height, std::vector<TextureAtlasTile>& tiles) {
	uint32_t x = getPadSize();
	uint32_t y = getPadSize();
	auto widthFloat = static_cast<float>(width);
	auto heightFloat = static_cast<float>(height);

	uint32_t rowStride = 0;
	for (auto&& tile : tiles) {
		auto aw = tile.getActualWidth();
		auto ah = tile.getActualHeight();
		auto w = tile.getWidth();
		auto h = tile.getHeight();
		
		if (ah > aw) {
			ah = aw;
		}
		if (h > w) {
			h = w;
		}

		uint32_t strideX = w + getPadSize() * 2;
		uint32_t strideY = h + getPadSize() * 2;

		rowStride = std::max(strideY, rowStride);

		if (y + h + getPadSize() > height) {
			return false; //woops won't fit, we failed :(
		}

		TextureUVCoordinateSet uvCoordSet = TextureUVCoordinateSet(
			x / widthFloat,
			y / heightFloat,
			(x + aw) / widthFloat,
			(y + ah) / heightFloat,
			width,
			height,
			tile.location
		);

		DEBUG_ASSERT(uvCoordSet._u0 >= 0.0f && uvCoordSet._u0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v0 >= 0.0f && uvCoordSet._v0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._u1 >= 0.0f && uvCoordSet._u1 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v1 >= 0.0f && uvCoordSet._v1 <= 1.0f, "Invalid UV");

		for (auto& tileUV : tile.uvs) {
			*tileUV = uvCoordSet;
		}

		x += strideX;
		if (x + w + getPadSize() >= width) {
			x = getPadSize();
			y += rowStride;
			rowStride = 0;
		}
	}
	return true;
};

inline int getIndex(int currentWidth, int x, int y) {
	return y * currentWidth * 2 + x;
}

void TextureAtlas::cleanTextureGroups() {
	const auto cleanTextureGroup = [] (std::map<ResourceLocation, UTexture2D*>& group) {
		for (auto& pair : group) {
			if (pair.second) {
				pair.second->RemoveFromRoot();
				pair.second->ConditionalBeginDestroy();
			}
		}
		group.clear();
	};
	cleanTextureGroup(mTextureGroup);
	cleanTextureGroup(mTextureGroupEmissive);
}

void TextureAtlas::redrawAtlas() {
	//desc.mMipMapCount = mMipCount;

	atlasTexture = UTexture2D::CreateTransient(mWidth, mHeight, EPixelFormat::PF_B8G8R8A8);

	// 	atlasTexture->MipGenSettings = TMGS_LeaveExistingMips;
	atlasTexture->AddressX = TextureAddress::TA_Wrap;
	atlasTexture->AddressY = TextureAddress::TA_Wrap;
	atlasTexture->Filter = TextureFilter::TF_Nearest;
	atlasTexture->PlatformData->NumSlices = 1;
	atlasTexture->NeverStream = true;

	int currentWidth = atlasTexture->GetSizeX();
	int currentHeight = atlasTexture->GetSizeY();
	int atlasTopWidth = currentWidth;
	int atlasTopHeight = currentHeight;

	for (int i = 1; i < mNumMipLevels; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;
		int currentBytes = currentWidth * currentHeight * 4;

		FTexture2DMipMap* Mip = new(atlasTexture->PlatformData->Mips) FTexture2DMipMap();
		Mip->SizeX = currentWidth;
		Mip->SizeY = currentHeight;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		Mip->BulkData.Realloc(currentBytes);
		Mip->BulkData.Unlock();
	}

	atlasTexture->UpdateResource();
	
	//D11.SC - if we have no actual emissive tiles, lets just use a tiny atlas
	bool ZeroEmissiveTextures = true;
	for (auto& tile : mTextureAtlasTiles) {
		if (tile.textureEmissive != nullptr) {
			ZeroEmissiveTextures = false;
			break;
		}
	}


	// Emissive atlas
	atlasTextureEmissive = UTexture2D::CreateTransient((ZeroEmissiveTextures) ? 1 : mWidth, (ZeroEmissiveTextures) ? 1 : mHeight, EPixelFormat::PF_B8G8R8A8);
	atlasTextureEmissive->AddressX = TextureAddress::TA_Wrap;
	atlasTextureEmissive->AddressY = TextureAddress::TA_Wrap;
	atlasTextureEmissive->Filter = TextureFilter::TF_Nearest;


	_renderAtlasLevel(0, mTextureAtlasTiles);

	// Build mipmaps
	for (int i = 1; i < mNumMipLevels; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;

		FTexture2DMipMap& MipSource = atlasTexture->PlatformData->Mips[i - 1];
		uint8* dataSource = (uint8*)MipSource.BulkData.Lock(LOCK_READ_ONLY);

		FTexture2DMipMap& MipDest = atlasTexture->PlatformData->Mips[i];
		uint8* dataDest = (uint8*)MipDest.BulkData.Lock(LOCK_READ_WRITE);

		for (int y = 0; y < currentHeight; y++) {
			for (int x = 0; x < currentWidth; x++) {
				int index = y * currentWidth + x;
				int xs = x * 2;
				int ys = y * 2;

				uint8 a1 = dataSource[getIndex(currentWidth, xs, ys) * 4 + 3] > 127 ? 1 : 0;
				uint8 a2 = dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 3] > 127 ? 1 : 0;
				uint8 a3 = dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 3] > 127 ? 1 : 0;
				uint8 a4 = dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 3] > 127 ? 1 : 0;
				int alphaCount = FMath::Max(a1 + a2 + a3 + a4, 1);
				dataDest[index * 4 + 0] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 0] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 0] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 0] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 0] * a4
					) / alphaCount;
				dataDest[index * 4 + 1] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 1] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 1] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 1] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 1] * a4
					) / alphaCount;
				dataDest[index * 4 + 2] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 2] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 2] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 2] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 2] * a4
					) / alphaCount;
				dataDest[index * 4 + 3] = alphaCount >= 2 ? 255 : 0;
			}
		}

		MipSource.BulkData.Unlock();
		MipDest.BulkData.Unlock();
	}

	atlasTexture->UpdateResource();
	atlasTextureEmissive->UpdateResource();
}


void TextureAtlas::redrawAtlasD11() {
	//desc.mMipMapCount = mMipCount;

	atlasTexture = UTexture2D::CreateTransient(mWidth, mHeight, EPixelFormat::PF_B8G8R8A8);

// 	atlasTexture->MipGenSettings = TMGS_LeaveExistingMips;
	atlasTexture->AddressX = TextureAddress::TA_Wrap;
	atlasTexture->AddressY = TextureAddress::TA_Wrap;
	atlasTexture->Filter = TextureFilter::TF_Nearest;
	atlasTexture->PlatformData->NumSlices = 1;
	atlasTexture->NeverStream = true;

	int currentWidth = atlasTexture->GetSizeX();
	int currentHeight = atlasTexture->GetSizeY();
	int atlasTopWidth = currentWidth;
	int atlasTopHeight = currentHeight;

	//D11.PS TODO FIX MIPS!
	/*
	//D11.PS - Might not need to do this anymore...
	for (int i = 1; i < mNumMipLevels; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;
		int currentBytes = currentWidth * currentHeight * 4;

		FTexture2DMipMap* Mip = new(atlasTexture->PlatformData->Mips) FTexture2DMipMap();
		Mip->SizeX = currentWidth;
		Mip->SizeY = currentHeight;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		Mip->BulkData.Realloc(currentBytes);
		Mip->BulkData.Unlock();
	}
	*/

	atlasTexture->UpdateResource();

	//D11.SC - if we have no actual emissive tiles, lets just use a tiny atlas
	bool ZeroEmissiveTextures = true;
	for (auto& tile : mTextureAtlasTiles) {
		if (tile.textureEmissive != nullptr) {
			ZeroEmissiveTextures = false;
			break;
		}
	}

	// Emissive atlas
	atlasTextureEmissive = UTexture2D::CreateTransient((ZeroEmissiveTextures) ? 1 : mWidth, (ZeroEmissiveTextures) ? 1 : mHeight, EPixelFormat::PF_B8G8R8A8);
	atlasTextureEmissive->AddressX = TextureAddress::TA_Wrap;
	atlasTextureEmissive->AddressY = TextureAddress::TA_Wrap;
	atlasTextureEmissive->Filter = TextureFilter::TF_Nearest;
	atlasTextureEmissive->UpdateResource(); //D11.PS

	//D11.PS - Create the source mips to copy to the texture
	std::vector< std::shared_ptr<uint8> > atlasMipSrc;
	atlasMipSrc.resize(mNumMipLevels);
	
	//D11.PS - Create the mip 0 and render the tiles to it
	atlasMipSrc[0] = std::shared_ptr<uint8>(new uint8[atlasTexture->GetSizeX() * atlasTexture->GetSizeY() * 4], std::default_delete<uint8[]>());
	_renderAtlasLevelD11(0, mTextureAtlasTiles, atlasMipSrc[0]);
	
	//D11.PS - Need to rebuild the mips properly taking into consideration swizzling 
	/*
	// Build mipmaps
	for (int i = 1; i < mNumMipLevels; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;

		atlasMipSrc[i] = new uint8[currentWidth * currentHeight * 4];
		FMemory::Memset(atlasMipSrc[i], 0, currentWidth * currentHeight * 4);

		uint8* dataSource = atlasMipSrc[i - 1];
		uint8* dataDest = atlasMipSrc[i];

		//FTexture2DMipMap& MipSource = atlasTexture->PlatformData->Mips[i - 1];
		//uint8* dataSource = (uint8*)MipSource.BulkData.Lock(LOCK_READ_ONLY);

		//FTexture2DMipMap& MipDest = atlasTexture->PlatformData->Mips[i];
		//uint8* dataDest = (uint8*)MipDest.BulkData.Lock(LOCK_READ_WRITE);

		for (int y = 0; y < currentHeight; y++) {
			for (int x = 0; x < currentWidth; x++) {
				int index = y * currentWidth + x;
				int xs = x * 2;
				int ys = y * 2;

				uint8 a1 = dataSource[getIndex(currentWidth, xs, ys) * 4 + 3] > 127 ? 1 : 0;
				uint8 a2 = dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 3] > 127 ? 1 : 0;
				uint8 a3 = dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 3] > 127 ? 1 : 0;
				uint8 a4 = dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 3] > 127 ? 1 : 0;
				int alphaCount = FMath::Max(a1 + a2 + a3 + a4, 1);
				dataDest[index * 4 + 0] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 0] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 0] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 0] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 0] * a4
					) / alphaCount;
				dataDest[index * 4 + 1] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 1] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 1] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 1] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 1] * a4
					) / alphaCount;
				dataDest[index * 4 + 2] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 2] * a1 +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 2] * a2 +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 2] * a3 +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 2] * a4
					) / alphaCount;
				dataDest[index * 4 + 3] = alphaCount >= 2 ? 255 : 0;
			}
		}


		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> DataCleanupFunc =
			[](uint8* SrcData, const FUpdateTextureRegion2D* Regions) {
			delete[] SrcData;
			delete Regions;
		};

		FUpdateTextureRegion2D *atlasRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, currentWidth, currentHeight);
		atlasTexture->UpdateTextureRegions(i, 1, atlasRegion, currentWidth * 4, 4, dataDest, DataCleanupFunc);

		//MipSource.BulkData.Unlock();
		//MipDest.BulkData.Unlock();
	}
	*/

	//	atlasTexture->UpdateResource();
	//	atlasTextureEmissive->UpdateResource();
}

void TextureAtlas::_addElement(Json::Value texture, ParsedAtlasNode& newNode, int atIndex, float mipFadeAmount, const Color& mipFadeColor) {
 	if (texture.isObject()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture["path"].asString());
 		auto overlayString = texture["overlay_color"].asString();
 		element.mipFadeAmount = texture["mip_fade_amount"].asFloat(mipFadeAmount);
 		element.mipFadeColor = mipFadeColor;
 		JsonUtil::parseValue(texture["mip_fade_color"], element.mipFadeColor);
 
 		if (overlayString.size() > 0) {
 			element.overlay = Color::fromARGB(strtoul(overlayString.substr(1).c_str(), nullptr, 16));
 			element.overlay.a = 1.f;
		
			newNode.elements[atIndex] = element;
		}
 	}
 	else if (texture.isString()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture.asString());
 		element.mipFadeAmount = mipFadeAmount;
 		element.mipFadeColor = mipFadeColor;
 		newNode.elements[atIndex] = element;
 	}
 	else {
 		DEBUG_FAIL("INVALID");
 	}
}

void TextureAtlas::_readNode(Json::Value jsonNode, ParsedAtlasNode& node, bool isRefreshing) {
	node.quad = jsonNode["quad"].asBool(false);
	Json::Value texture = jsonNode["textures"];
	float defaultFadeMipAmount = jsonNode["mip_fade_amount"].asFloat();
	Color defaultFadeMipColor = Color::NIL;
	JsonUtil::parseValue(jsonNode["mip_fade_color"], defaultFadeMipColor);

	if (texture.isArray()) {
		if (!isRefreshing) {
			node.elements.resize(texture.size());
		}

		size_t elementIndex = 0;
		for (auto& textureElement : texture) {
			_addElement(textureElement, node, elementIndex, defaultFadeMipAmount, defaultFadeMipColor);
			++elementIndex;
			// This can happen if an override of a vanilla node has more elements in it than we expect
			if (elementIndex >= node.elements.size()) {
				break;
			}
		}
	}
	else {
		if (!isRefreshing) {
			node.elements.resize(1);
		}

		_addElement(texture, node, 0, defaultFadeMipAmount, defaultFadeMipColor);
	}
}

void TextureAtlas::_loadAtlasNodes(Json::Value root, std::vector<ParsedAtlasNode>& atlasNodes) {

	for (auto& nodeName : root.getMemberNames()) {
		ParsedAtlasNode newNode;
		Json::Value jsonNode = root[nodeName];

		newNode.name = nodeName;
		_readNode(jsonNode, newNode, false);

		atlasNodes.push_back(newNode);
	}
}

void TextureAtlas::_addTile(std::vector<TextureAtlasTile> &tiles, TextureUVCoordinateSet* uvSet, bool quad, float fadePerMipAmount, const Color& fadeMipColor, ResourceLocation& location, const Color& overlayColor, const ResourcePack& resource) {
	bool found = false;
	for (auto& tileInstance : tiles) {
		if (tileInstance.overlay == overlayColor &&
			tileInstance.quad == quad &&
			tileInstance.location == location &&
			tileInstance.mipFadeRate == fadePerMipAmount &&
			tileInstance.mipFadeColor == fadeMipColor) {

			found = true;
			tileInstance.uvs.push_back(uvSet);
		}
	}

	if (!found) {
		TextureAtlasTile tile(overlayColor, quad, location, resource);
		tile.mipFadeRate = fadePerMipAmount;
		tile.mipFadeColor = fadeMipColor;
		tiles.emplace_back(std::move(tile));
		tiles.back().uvs.push_back(uvSet);
	}
}

void TextureAtlas::loadMetaFile(const ResourcePack& resource) {
	mTextureItems.clear();
	mTextureAtlasTiles.clear();
	mAtlasNodesList.clear();
	mTextureGroup.clear();
	mTextureGroupEmissive.clear();

	{
		const auto json = resource.getJson("resources.json");
		if (!json) {
			return;
		}
		Json::Value root = json.GetValue();

		auto textureList = root["resources"]["textures"];
		for (auto& nodeName : textureList.getMemberNames()) {
			mResourceMap[nodeName] = textureList[nodeName].asString();
		}
	}
	const auto json = resource.getJson("images/terrain_texture.json");
	if (!json) {
		return;
	}
	Json::Value root = json.GetValue();

	mPadSize = root.get("padding", 0).asInt();
	mNumMipLevels = root.get("num_mip_levels", 4).asInt();

	mResourceName = root["texture_name"].asString();

	_loadAtlasNodes(root["texture_data"], mAtlasNodesList);

	// Now that we have the properly overridden nodes,
	// we can use that list to generate the list of tiles
	for (auto& atlasNode : mAtlasNodesList) {
		for (auto& atlasNodeEntry : atlasNode.elements) {
			_addTile(mTextureAtlasTiles, &atlasNodeEntry.uv, atlasNode.quad, atlasNodeEntry.mipFadeAmount, atlasNodeEntry.mipFadeColor, atlasNodeEntry.location, atlasNodeEntry.overlay, resource);
		}
	}

	//TODO how to guess the sizes on the server without loading the textures? Is it even needed? Is any of this needed?
	for (auto&& tile : mTextureAtlasTiles) {
		bool isValid = tile.loadTexture(mTextureGroup, mResourceMap);
		if (!isValid) {
			UE_LOG(LogTemp, Warning, TEXT("Could not load texture: %s"), UTF8_TO_TCHAR(tile.location.mPath.c_str()));
		}
		tile.loadTextureEmissive(mTextureGroupEmissive, mResourceMap);
	}

	mTextureAtlasTiles.erase(
		std::remove_if(
			mTextureAtlasTiles.begin(),
			mTextureAtlasTiles.end(),
			[](TextureAtlasTile& element) -> bool {
		return element.texture == nullptr;
	}
		),
		mTextureAtlasTiles.end()
		);

	mWidth = mHeight = 1;
	uint32_t area = 0;
	for (auto&& tile : mTextureAtlasTiles) {
		area += (tile.getWidth() + 2 * getPadSize()) * (tile.getHeight() + 2 * getPadSize());
	}

	while (mWidth * mHeight < area) {
		//double the smallest between the dimensions or width
		select_min(mWidth, mHeight) *= 2;
	}

	//assign the uvs to all textures based on their size
	//sort the tiles based on their size, biggest last
	std::stable_sort(mTextureAtlasTiles.begin(), mTextureAtlasTiles.end(), [](const TextureAtlasTile& A, const TextureAtlasTile& B) {
		return A.getArea() < B.getArea();
	});

	//try to fit all tiles inside the assigned sizes, doubling the size and retrying if it fails
	while (!_tryAssignUVs(mWidth, mHeight, mTextureAtlasTiles)) {
		select_min(mWidth, mHeight) *= 2;
	}

	// Now that all the uv's have been generated, we need to build the mTextureItems list
	int nodeIndex = 0;
	for (auto& atlasNode : mAtlasNodesList) {
		TextureAtlasItem newAtlasItem(atlasNode.name, atlasNode.elements.size());
		newAtlasItem.setParsedNodeIndex(nodeIndex);

		int uvIndex = 0;
		for (auto& atlasNodeEntry : atlasNode.elements) {
			newAtlasItem[uvIndex] = atlasNodeEntry.uv;
			++uvIndex;
		}
		mTextureItems[atlasNode.name] = newAtlasItem;
		++nodeIndex;
	}
}

const TextureAtlasItem& TextureAtlas::getTextureItem(const std::string& textureName) const {
	auto tex = mTextureItems.find(textureName);
	//DEBUG_ASSERT(tex != mTextureItems.end(), "This texture item doesn't exist");
	if (tex == mTextureItems.end()) {
		return mMissingTextureItem;
	}
	return tex->second;
}

FVector2D TextureAtlas::getAtlasFaceSize() const {
	return mAtlasFaceSize;
}

int TextureAtlas::getPadSize() const {
	return mPadSize;
}

void TextureAtlas::onAppResumed() {
// 	mce::RenderContextImmediate::get().beginFrame();
#if PLATFORM_WINDOWS //D11.PS
	redrawAtlas();
#else
	redrawAtlasD11();
#endif
// 	mce::RenderContextImmediate::get().endFrame();
}

void renderSingleTile(uint8* atlasData, int atlasWidth, int atlasHeight, const TextureUVCoordinateSet& uvs, UTexture2D* texture, int padSize, const Color& overlay, bool quad, bool premultiplySrcAlpha = false) {
	// TODO: fix quad == true remap (water/lava)

	FTexture2DMipMap& Mip = texture->PlatformData->Mips[0];
	const uint8* textureData = (uint8*)Mip.BulkData.LockReadOnly();

	int textureW = texture->GetSizeX();
	int textureH = texture->GetSizeY();

#if PLATFORM_WINDOWS
	const float UVOffset = 0.0f;
#else
	//D11.PS - added +0.5f because of rounding irregularities with PS4 DebugGame and other configs.
	const float UVOffset = 0.5f;
#endif

	int atlasX = static_cast<int>((uvs._u0 * (float)atlasWidth) + UVOffset);
	int atlasY = static_cast<int>((uvs._v0 * (float)atlasHeight) + UVOffset);

	const float INV255 = 1.f / 255.f;

	if (premultiplySrcAlpha)
	{
		if (overlay.a > 0)
		{
			const uint8* pSrc = textureData;
			uint8* pDst = atlasData + (((atlasY * atlasWidth) + atlasX) * 4);
			const uint32 AtlasStride = ((atlasWidth - textureW) * 4);

			for (int y = 0; y < textureH; ++y) {
				for (int x = 0; x < textureW; ++x) {
					if (pSrc[3] > 0)
					{
						const float fSrcAlpha = (float)pSrc[3] * INV255;
						Color mixedColor(((float)pSrc[2] * INV255) * fSrcAlpha, ((float)pSrc[1] * INV255) * fSrcAlpha, ((float)pSrc[0] * INV255) * fSrcAlpha, fSrcAlpha);
						mixedColor = Color::lerp(mixedColor, mixedColor * overlay, mixedColor.a * overlay.a);

						pDst[0] = (uint8)(mixedColor.b * 255.0f);
						pDst[1] = (uint8)(mixedColor.g * 255.0f);
						pDst[2] = (uint8)(mixedColor.r * 255.0f);
						pDst[3] = pSrc[3];
					}
					pSrc += 4;
					pDst += 4;
				}
				pDst += AtlasStride;
			}
		}
		else
		{
			const uint8* pSrc = textureData;
			uint8* pDst = atlasData + (((atlasY * atlasWidth) + atlasX) * 4);
			const uint32 AtlasStride = ((atlasWidth - textureW) * 4);

			for (int y = 0; y < textureH; ++y) {
				for (int x = 0; x < textureW; ++x) {
					if (pSrc[3] > 0)
					{
						const float fSrcAlpha = (float)pSrc[3];
						pDst[0] = (uint8)(((float)pSrc[0] * INV255) * fSrcAlpha);
						pDst[1] = (uint8)(((float)pSrc[1] * INV255) * fSrcAlpha);
						pDst[2] = (uint8)(((float)pSrc[2] * INV255) * fSrcAlpha);
						pDst[3] = pSrc[3];
					}
					pSrc += 4;
					pDst += 4;
				}
				pDst += AtlasStride;
			}
		}
	}
	else
	{
		if (overlay.a > 0)
		{
			const uint8* pSrc = textureData;
			uint8* pDst = atlasData + (((atlasY * atlasWidth) + atlasX) * 4);
			const uint32 AtlasStride = ((atlasWidth - textureW) * 4);

			for (int y = 0; y < textureH; ++y) {
				for (int x = 0; x < textureW; ++x) {
					if (pSrc[3] > 0)
					{
						Color mixedColor((float)pSrc[2] * INV255, (float)pSrc[1] * INV255, (float)pSrc[0] * INV255, (float)pSrc[3] * INV255);
						mixedColor = Color::lerp(mixedColor, mixedColor * overlay, mixedColor.a * overlay.a);

						pDst[0] = (uint8)(mixedColor.b * 255.0f);
						pDst[1] = (uint8)(mixedColor.g * 255.0f);
						pDst[2] = (uint8)(mixedColor.r * 255.0f);
						pDst[3] = pSrc[3];
					}
					pSrc += 4;
					pDst += 4;
				}
				pDst += AtlasStride;
			}
		}
		else
		{
			const uint8* pSrc = textureData;
			uint8* pDst = atlasData + (((atlasY * atlasWidth) + atlasX) * 4);
			const uint32 AtlasStride = atlasWidth * 4;
			const uint32 LineStride = textureW * 4;
			for (int y = 0; y < textureH; ++y) {
				FMemory::Memcpy(pDst, pSrc, LineStride);
				pSrc += LineStride;
				pDst += AtlasStride;
			}
		}
	}


	Mip.BulkData.Unlock();
}

void TextureAtlas::_renderAtlasLevel(uint32_t mipID, std::vector<TextureAtlasTile>& tiles) {
	TextureAtlasTile* dirtTile = nullptr;
	std::vector<TextureAtlasTile*> grassSides;
	for (auto& tile : tiles) {
		if (tile.location.mPath == "block.dirt") {
			dirtTile = &tile;
		}
		else if (tile.location.mPath == "block.grass.side") {
			grassSides.emplace_back(&tile);
		}
	}

	FTexture2DMipMap& AtlasMip = atlasTexture->PlatformData->Mips[0];
	uint8* atlasData = (uint8*)AtlasMip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasData, 0, atlasTexture->GetSizeX() * atlasTexture->GetSizeY() * 4);

	FTexture2DMipMap& AtlasMipEmissive = atlasTextureEmissive->PlatformData->Mips[0];
	uint8* atlasDataEmissive = (uint8*)AtlasMipEmissive.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasDataEmissive, 0, atlasTextureEmissive->GetSizeX() * atlasTextureEmissive->GetSizeY() * 4);

	
	if (dirtTile != nullptr) {
		for (auto* tile : grassSides) {
			renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile->uvs[0], dirtTile->texture, getPadSize(), dirtTile->overlay, dirtTile->quad);
		}
	}

	for (auto& tile : tiles) {
		renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile.uvs[0], tile.texture, getPadSize(), tile.overlay, tile.quad);

		if (tile.textureEmissive != nullptr) {
			if (tile.location.mPath == "block.redstone.torch.on" || tile.location.mPath == "block.torch.on") //D11.SC Rather than have to update all the packages/textures for the torch emissive, just fix them by premultiplying
			{
				renderSingleTile(atlasDataEmissive, atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad, true);
			}
			else
			{
				renderSingleTile(atlasDataEmissive, atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad);
			}
			
		}
		
	}

	AtlasMipEmissive.BulkData.Unlock();
	AtlasMip.BulkData.Unlock();
}

//D11.PS - Temp D11 version
void TextureAtlas::_renderAtlasLevelD11(uint32_t mipID, std::vector<TextureAtlasTile>& tiles, std::shared_ptr<uint8> atlasSrc) {
	TextureAtlasTile* dirtTile = nullptr;
	std::vector<TextureAtlasTile*> grassSides;
	for (auto& tile : tiles) {
		if (tile.location.mPath == "block.dirt") {
			dirtTile = &tile;
		}
		else if (tile.location.mPath == "block.grass.side") {
			grassSides.emplace_back(&tile);
		}
	}

	//D11.PS - Source atlas data
	FMemory::Memset(atlasSrc.get(), 0, atlasTexture->GetSizeX() * atlasTexture->GetSizeY() * 4);

	//D11.PS - create the source data for emissive 
	std::shared_ptr<uint8> emissiveAtlasShared = std::shared_ptr<uint8>(new uint8[atlasTextureEmissive->GetSizeX() * atlasTextureEmissive->GetSizeY() * 4], std::default_delete<uint8[]>());
	FMemory::Memset(emissiveAtlasShared.get(), 0, atlasTextureEmissive->GetSizeX() * atlasTextureEmissive->GetSizeY() * 4);


	if (dirtTile != nullptr) {
		for (auto* tile : grassSides) {
			renderSingleTile(atlasSrc.get(), atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile->uvs[0], dirtTile->texture, getPadSize(), dirtTile->overlay, dirtTile->quad);
		}
	}

	for (auto& tile : tiles) {
		renderSingleTile(atlasSrc.get(), atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile.uvs[0], tile.texture, getPadSize(), tile.overlay, tile.quad);
		if (tile.textureEmissive != nullptr) {
			if (tile.location.mPath == "block.redstone.torch.on" || tile.location.mPath == "block.torch.on") //D11.SC Rather than have to update all the packages/textures for the torch emissive, just fix them by premultiplying
			{
				renderSingleTile(emissiveAtlasShared.get(), atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad, true);
			}
			else
			{
				renderSingleTile(emissiveAtlasShared.get(), atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad);
			}
		}
	}
	
	//D11.PS - update the texture regions
	FUpdateTextureRegion2D *atlasRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, atlasTexture->GetSizeX(), atlasTexture->GetSizeY());
	atlasTexture->UpdateTextureRegions(0, 1, atlasRegion, atlasTexture->GetSizeX() * 4, 4, atlasSrc.get(), [atlasSrc](uint8* SrcData, const FUpdateTextureRegion2D* Regions) {
		delete Regions;
	});

	FUpdateTextureRegion2D *emissiveRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY());
	atlasTextureEmissive->UpdateTextureRegions(0, 1, emissiveRegion, atlasTextureEmissive->GetSizeX() * 4, 4, emissiveAtlasShared.get(), [emissiveAtlasShared](uint8* SrcData, const FUpdateTextureRegion2D* Regions) {
		delete Regions;
	});

	

	//AtlasMipEmissive.BulkData.Unlock();
	//AtlasMip.BulkData.Unlock();
}