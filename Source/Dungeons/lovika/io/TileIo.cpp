#include "Dungeons.h"
#include "TileIo.h"
#include "lovika/tile/TileDef.h"
#include "lovika/BlockRegionUtil.h"
#include "lovika/BlockPosTransform.h"
#include "game/level/doors/DoorUtil.h"

//
// ObjectGroup -> Tile/Prop groups
//
static DoorDef parseDoor(const io::Object& object, const io::Door& door) {
	const auto& bounds = door.bounds;
	const auto pos = (bounds.minInclusive + bounds.maxExclusive) / 2;
	const auto size = bounds.size();
	return DoorDef(pos, game::door::probableFacing(bounds, BlockCuboid::fromSize(object.blocks->size())), std::max(size.x, size.z), door.name, door.tags);
}

static std::vector<DoorDef> parseDoors(const io::Object& object) {
	std::vector<DoorDef> doors;
	for (auto& doorDef : object.doors) {
		doors.push_back(parseDoor(object, doorDef));
	}
	return doors;
}

static lovika::Region parseRegion(const io::Object& object, const io::Region& region) {
	return lovika::Region(region.name, region.type, region.bounds, region.tags);
}

static std::vector<lovika::Region> parseRegions(const io::Object& object) {
	std::vector<lovika::Region> regions;
	for (auto& regionDef : object.regions) {
		regions.push_back(parseRegion(object, regionDef));
	}
	return regions;
}

static Boundary parseBoundary(const io::Object& object, const io::Boundary& boundary) {
	return { boundary.position, boundary.height };
}

static std::vector<Boundary> parseBoundaries(const io::Object& object) {
	std::vector<Boundary> boundaries;
	for (auto&& boundary : object.boundaries) {
		boundaries.push_back(parseBoundary(object, boundary));
	}
	return boundaries;
}

static std::vector<RegionCell> parseRegionPlane(const io::Object& object) {
	std::vector<RegionCell> cells;

	const auto& size = object.blocks->size();

	int p { 0 };
	for (auto z = 0; z < size.z; z++) {
		for (auto x = 0; x < size.x; x++) {
			const auto value = object.regionPlane[p];
			const auto y = object.maxHeightOfRegionTerrainPlane.empty()? 0 : object.maxHeightOfRegionTerrainPlane[p];
			cells.push_back({ BlockPos { x, y, z }, value });
			p++;
		}
	}

	return cells;
}

static std::vector<WalkableCell> parseWalkablePlane(const io::Object& object, const std::vector<WalkableHeight>& walkablePlane) {
	const auto& size = object.blocks->size();

	if (walkablePlane.size() != size.productXz()) {
		return {};
	}
	std::vector<WalkableCell> cells;

	int p { 0 };
	for (auto z = 0; z < size.z; z++) {
		for (auto x = 0; x < size.x; x++) {
			const auto value = walkablePlane[p];
			cells.push_back({ BlockPos { x, 0, z }, WalkableHeight { value } });
			p++;
		}
	}

	return cells;
}

static TOptional<Height> calculateLowestWalkableBlockY(const std::vector<WalkableCell>& walkableHeights) {
	TOptional<Height> lowest;

	for (auto& height : walkableHeights) {
		const auto walkableY = height.value.toHeightThreshold();
		if (walkableY && (!lowest || walkableY.GetValue() < lowest.GetValue())) {
			lowest = walkableY.GetValue();
		}
	}
	return lowest;
}

io::tile::Objects io::tile::fromObjectGroup(const io::ObjectGroup& group) {
	Objects objects;

	for (auto& object : group.objects) {
		auto o = std::make_unique<TileDef>(
			object.id,
			group.def.name.id,
			*object.blocks,
			::parseDoors(object),
			::parseRegions(object),
			::parseBoundaries(object),
			::parseRegionPlane(object),
			::parseWalkablePlane(object, object.walkablePlane),
			calculateLowestWalkableBlockY(::parseWalkablePlane(object, object.walkablePlaneFromLovika)),
			object.tags
		);
		o->setBaseY(object.y);
		o->setPosHACK(object.bounds.minInclusive);

		if (object.isTile()) {
			objects.tiles.push_back(std::move(o));
		} else {
			objects.props.push_back(std::move(o));
		}
	}
	return objects;
}
