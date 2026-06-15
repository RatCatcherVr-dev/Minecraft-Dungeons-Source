#include "Dungeons.h"
#include "Resource.h"
#include "util/EnumUtil.h"

namespace internal
{
std::string convertToString(const ResourcePack::Blob& resourceStream) {
	return std::string((const char*)resourceStream.GetData(), resourceStream.Num());
}

}
const ResourcePack ResourcePack::DEFAULT("creeperwoods");

ResourcePack::ResourcePack(const FString& resourcePackFolder)
	: BasePath(FPaths::ProjectDir() + TEXT("Content/data/resourcepacks/") + resourcePackFolder + "/") {
}

ResourcePack::ResourcePack(const EResourcePack& resourcePackFolder)
	: ResourcePack(GetEnumValueToStringStripped(resourcePackFolder)){
}

TOptional<Json::Value> ResourcePack::getJson(std::string path) const {
	ResourceLocation resourceLocation(path);
	
	Blob resourceBlob;
	Json::Value root;
	Json::Reader reader;

	auto fpath = getFullPathFor(resourceLocation);
	FFileHelper::LoadFileToArray(resourceBlob, *fpath);
	auto parseString = internal::convertToString(resourceBlob);

	if (!reader.parse(parseString, root)) {
		return {};
	}
	return root;
}

FString ResourcePack::getFullPathFor(const ResourceLocation& resourceLocation) const {
	return BasePath + FString(resourceLocation.mPath.c_str());
}

const FString& ResourcePack::getBasePath() const {
	return BasePath;
}

