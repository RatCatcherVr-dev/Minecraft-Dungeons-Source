#include "Dungeons.h"
#include "DoorDef.h"
#include "lovika/BlockPosTransform.h"
#include "lovika/QuadrantAngle.h"

DoorDef::DoorDef(const BlockPos& position, FacingID facing, int width, const std::string& name, const std::string& tagString, TOptional<FString> prefab/*= {}*/)
	: mPosition(position)
	, mNeighbourPosition(position.neighbor(facing))
	, mWidth(width)
	, mFacing((Facing::Name) facing)
	, mNeighbourFacing(static_cast<Facing::Name>(Facing::OPPOSITE_FACING[facing]))
	, mName(name)
	, mLowerName(Util::toLower(name))
	, mLowerTagString(Util::toLower(tagString))
	, prefab(std::move(prefab))
{
	//checkf((width % 2) == 1, TEXT("Width must be odd!"));
}

BlockPos DoorDef::position() const {
	return mPosition;
}

int DoorDef::width() const {
	return mWidth;
}

BlockPos DoorDef::neighbourPos() const {
	return mNeighbourPosition;
}

Facing::Name DoorDef::neighbourFacing() const {
	return mNeighbourFacing;
}

bool DoorDef::isSpatiallyAdjacent(const DoorDef& rhs) const {
	return positions().intersection(rhs.positions()) != BlockCuboid{};
}

Facing::Name DoorDef::facing() const {
	return mFacing;
}

static FacingID perpendicularFacing(FacingID facing) {
	return facing + QuadrantAngle::D90;
}

std::pair<BlockPos, BlockPos> DoorDef::doorFrame() const {
	auto facing = perpendicularFacing(mFacing);
	auto steps = 1 + (mWidth / 2);
	return std::make_pair(mPosition.relative(facing, -steps), mPosition.relative(facing, steps));
}

BlockCuboid DoorDef::positions() const {
	auto facing = perpendicularFacing(mFacing);
	auto steps = mWidth / 2;
	auto a = mPosition.relative(facing, -steps);
	auto b = mPosition.relative(facing, steps);
	return BlockCuboid::fromInclusiveCorners(a, b);
}

bool DoorDef::nameMatches(const std::string& name) const {
	return Util::toLower(name) == mLowerName;
}

const std::string& DoorDef::name() const {
	return mName;
}

bool DoorDef::operator==(const DoorDef& rhs) const {
	return mFacing == rhs.mFacing && mPosition == rhs.mPosition && mWidth && rhs.mWidth;
}

bool DoorDef::operator!=(const DoorDef& rhs) const {
	return !operator==(rhs);
}

const std::string& DoorDef::lowerTagString() const {
	return mLowerTagString;
}

DoorDef transformed(const DoorDef& door, const BlockPosTransform& transform) {
	return { transform(door.position()), door.facing() + blockpostransform::rotation(transform), door.width(), door.name(), door.lowerTagString(), door.prefab };
}

std::vector<DoorDef> transformed(const std::vector<DoorDef>& doors, const BlockPosTransform& transform) {
	std::vector<DoorDef> out;
	out.reserve(doors.size());
	for (auto&& door : doors) {
		out.push_back(transformed(door, transform));
	}
	return out;
}
