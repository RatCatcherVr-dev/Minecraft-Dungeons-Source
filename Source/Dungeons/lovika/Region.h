#pragma once

#include "RegionType.h"
#include "BlockCuboid.h"
#include "util/BooleanOperatorMacros.h"

namespace lovika {

class Region {
public:
	Region(const std::string& name, const RegionType&, const BlockCuboid&, const std::string& tagString);

	const RegionType& type() const;
	const std::string& name() const;
	const std::string& lowerName() const;
	const BlockCuboid& area() const;
	int y() const;
	const std::string& tagString() const;
	const std::string& lowerTagString() const;

	bool operator==(const Region&) const;
	operator const BlockCuboid&() const;
private:
	const RegionType* mType;
	std::string mName;
	std::string mLowerName;
	std::string mTagString;
	std::string mLowerTagString;
	BlockCuboid mArea;
};
}


using RegionPredicate = Pred<const lovika::Region&>;
GENERATE_BOOLEAN_PRED_OPERATORS(const lovika::Region&);

std::vector<lovika::Region> filtered(const std::vector<lovika::Region>&, const RegionPredicate&);

lovika::Region transformed(const lovika::Region&, const BlockPosTransform&);
std::vector<lovika::Region> transformed(const std::vector<lovika::Region>&, const BlockPosTransform&);

namespace std {
template <>
struct hash<lovika::Region> {
	std::size_t operator()(const lovika::Region& region) const {
		return hash<BlockCuboid>()(region.area());
	}
};
}