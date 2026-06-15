#include "legacy/Core/pch_core.h"

#include "ResourceHelper.h"
//#include "Core/Resource/TextureData.h"
//#include "Core/Debug/DebugUtils.h"
#include "legacy/Core/Utility/EnumCast.h"

static const std::array<std::string, enum_cast(ResourceInformation::ResourceType::Count)> ResourceTypeToStr = {
	"invalid",
	"resources",
	"data",
	"plugin",
	"interface",
	"mandatory"
};

const std::string& ResourceInformation::StringFromResourceType(ResourceType value) {
	auto intValue = enum_cast(value);
	if (static_cast<size_t>(intValue) < ResourceTypeToStr.size() && intValue >= 0) {
		return ResourceTypeToStr[intValue];
	}
	return ResourceTypeToStr.front();
}

ResourceInformation::ResourceType ResourceInformation::ResourceTypeFromString(const std::string& value) {
	auto typeItr = std::find(ResourceTypeToStr.begin(), ResourceTypeToStr.end(), value);
	if (typeItr != ResourceTypeToStr.end()) {
		return static_cast<ResourceType>(typeItr - ResourceTypeToStr.begin());
	}
	return ResourceType::Invalid;
}

ResourceInformation::ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, const std::string& type, const std::string& entry) 
	: mDescription(description)
	, mVersion(version)
	, mUUID(uuid)
	, mType(ResourceTypeFromString(type))
	, mEntry(entry) {
}

ResourceInformation::ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, ResourceType type /*= ResourceType::Invalid*/, const std::string& entry /*= ""*/)
	: mDescription(description)
		, mVersion(version)
		, mUUID(uuid)
		, mType(type)
		, mEntry(entry) {
}

bool PackIdVersion::operator==(const PackIdVersion& rhs) const {
	return (mId == rhs.mId) && (mVersion == rhs.mVersion);
}
