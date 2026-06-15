#include "Dungeons.h"
#include "Region.h"

namespace lovika {

Region::Region(const std::string& name, const RegionType& type, const BlockCuboid& area, const std::string& tagString)
	: mName(name)
	, mLowerName(Util::toLower(name))
	, mType(&type)
	, mArea(area)
	, mTagString(tagString)
	, mLowerTagString(Util::toLower(tagString)) {
}

const BlockCuboid& Region::area() const {
	return mArea;
}

int Region::y() const {
	return mArea.minInclusive.y;
}

const std::string& Region::name() const {
	return mName;
}

const std::string& Region::lowerName() const {
	return mLowerName;
}

const std::string& Region::tagString() const {
	return mTagString;
}

const std::string& Region::lowerTagString() const {
	return mLowerTagString;
}

const RegionType& Region::type() const {
	return *mType;
}

bool Region::operator==(const Region& rhs) const {
	return std::tie(mType, mArea, mLowerName, mLowerTagString) == std::tie(rhs.mType, rhs.mArea, rhs.mLowerName, mLowerTagString);
}

Region::operator const BlockCuboid&() const {
	return mArea;
}

}
//
// Helpers
//
std::vector<lovika::Region> filtered(const std::vector<lovika::Region>& regions, const RegionPredicate& pred) {
	std::vector<lovika::Region> out;
	for (auto&& region : regions) {
		if (pred(region)) {
			out.push_back(region);
		}
	}
	return out;
}

lovika::Region transformed(const lovika::Region& region, const BlockPosTransform& transform) {
	return{ region.name(), region.type(), transformed(region.area(), transform), region.tagString() };
}

std::vector<lovika::Region> transformed(const std::vector<lovika::Region>& regions, const BlockPosTransform& transform) {
	std::vector<lovika::Region> out;
	out.reserve(regions.size());
	for (auto&& region : regions) {
		out.push_back(transformed(region, transform));
	}
	return out;
}
