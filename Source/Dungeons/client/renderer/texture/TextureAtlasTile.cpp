#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlasTile.h"
//#include "Renderer/HAL/Interface/Texture.h"
//#include "Renderer/TextureGroup.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "client/resource/Resource.h"
#include "lovika/io/TextureFile.h"
#include <IImageWrapperModule.h>
#include <IImageWrapper.h>

TextureAtlasTile::TextureAtlasTile(Color overlay, bool quad, ResourceLocation location, const ResourcePack& resource)
	: overlay(overlay)
	, quad(quad)
	, location(location)
	, texture(nullptr)
	, textureEmissive(nullptr)
	, textureReflective(nullptr)
	, resourcePack(resource) {
}

TextureAtlasTile::TextureAtlasTile(TextureAtlasTile &&rhs)
	: overlay(rhs.overlay)
	, quad(rhs.quad)
	, location(rhs.location)
	, mipFadeRate(rhs.mipFadeRate)
	, mipFadeColor(rhs.mipFadeColor)
	, uvs(std::move(rhs.uvs))
	, texture(rhs.texture)
	, textureEmissive(rhs.textureEmissive)
	, textureReflective(rhs.textureReflective)
	, resourcePack(rhs.resourcePack) {
}

TextureAtlasTile& TextureAtlasTile::operator=(TextureAtlasTile &&rhs) {
	if (this != &rhs) {
		overlay = rhs.overlay;
		quad = rhs.quad;
		location = rhs.location;
		mipFadeRate = rhs.mipFadeRate;
		mipFadeColor = rhs.mipFadeColor;
		uvs = rhs.uvs;
		texture = rhs.texture;
		textureEmissive = rhs.textureEmissive;
		textureReflective = rhs.textureReflective;
		resourcePack = rhs.resourcePack;
	}
	return *this;
}

bool _loadTexture(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources, const ResourceLocation& location, const ResourcePack& resourcePack, UTexture2D** result) {
	if (textures[location] != nullptr) {
		*result = textures[location];
		return true;
	}

	std::string fileName = resources[location.mPath];
	if (fileName.empty()) {
		return false;
	}
	EImageFormat imageFormat = EImageFormat::PNG;
	if (Util::endsWith(fileName, ".jpg")) {
		imageFormat = EImageFormat::JPEG;
	}

	FString pathLocation = resourcePack.getFullPathFor(ResourceLocation(fileName.c_str()));

	bool isValid;
	uint32_t width;
	uint32_t height;
	*result = io::loadTexture(pathLocation, imageFormat, isValid, width, height);
	if (isValid) {
		textures[location] = *result;
	}
	return isValid;
}

bool TextureAtlasTile::loadTexture(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources) {
	return _loadTexture(textures, resources, location, resourcePack, &texture);
}

bool TextureAtlasTile::loadTextureEmissive(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources) {
	ResourceLocation emissiveLocation(location.mPath + ".emissive");
	return _loadTexture(textures, resources, emissiveLocation, resourcePack, &textureEmissive);
}

bool TextureAtlasTile::loadTextureReflective(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources) {
	ResourceLocation reflectiveLocation(location.mPath + ".reflective");
	return _loadTexture(textures, resources, reflectiveLocation, resourcePack, &textureReflective);
}

uint32_t TextureAtlasTile::getActualWidth() const {
	return std::max((uint32_t)texture->GetSizeX(), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getActualHeight() const {
	return std::max((uint32_t)texture->GetSizeY(), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getWidth() const {
	return std::max((uint32_t)texture->GetSizeX() * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getHeight() const {
	return std::max((uint32_t)texture->GetSizeY() * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getArea() const {
	return getWidth() * getHeight();
}