#include "Dungeons.h"

#include "ObjectGroupFile.h"
#include "lovika/world/level/terrain/TerrainCell.h"
#include "util/Base64.h"
#include "util/ZLibUtil.h"
#include "util/Algo.h"
#include "Async/ParallelFor.h"

static Unique<ZLibUtil> mCompressor = make_unique<ZLibUtil>(7);

/// SAVE ///

namespace io {
using namespace std;

static Json::Value toJson(const BlockPos& bp) {
    Json::Value node;
    node.append(bp.x);
    node.append(bp.y);
    node.append(bp.z);
    return node;
}

static Json::Value toJson(const Region& region) {
    Json::Value node;
    node["name"] = region.name;
    node["pos"] = toJson(region.bounds.minInclusive);
    node["size"] = toJson(region.bounds.size());
	node["type"] = region.type.name;
	node["tags"] = region.tags;
    return node;
}

static Json::Value toJson(const Door& door) {
    Json::Value node;
    node["name"] = door.name;
    node["pos"] = toJson(door.bounds.minInclusive);
    node["size"] = toJson(door.bounds.size());
	node["tags"] = door.tags;
	return node;
}

static Json::Value toJson(const BlockRegion& blocks, int specialAirLevel) {
	BlockID BLOCK_ID_END_STONE = BlockID(121); // Block::mEndStone->getId();
    BlockRegion::RegionSize size = blocks.size();
    size_t nBlocks = (size_t) size.product();
    size_t nBytes = (nBlocks * 12 + 4) / 8;

    string rawData(nBytes, 0);

    //TODO: DF convert to iterator?
    for (int y = 0, i = 0; y < size.y; y++) {
        bool useSpecialAir = y < specialAirLevel;
        for (int z = 0; z < size.z; z++) {
            for (int x = 0; x < size.x; x++, i++) {
                FullBlock block = blocks.getBlock(x, y, z);
                auto blockId = block.id;
                auto blockData = block.data;

                if (useSpecialAir && blockId == BlockID::AIR) {
                    blockData = 0xF;
                }
                if (blockId == BLOCK_ID_END_STONE) {
                    blockId = BlockID::AIR;
                }
                // Set the blockId and blockData
                rawData[i] = blockId;
                rawData[nBlocks + i / 2] |= (i & 1) ? blockData : blockData << 4;
            }
        }
    }

    // compress and base64 encode
    string compressed;
	mCompressor->compress(rawData, compressed);
    return Util::base64_encode(compressed);
}

static Json::Value toJson(const Object& object) {
    Json::Value node;
    node["id"] = object.id;
	node["pos"] = toJson(object.bounds.minInclusive);
	node["size"] = toJson(object.bounds.size());
	node["y"] = object.y;
	node["tags"] = object.tags;

    if (!object.doors.empty()) {
        Json::Value doorNodes;
        for (auto& door : object.doors) {
            doorNodes.append(toJson(door));
        }
        node["doors"] = doorNodes;
    }

    if (!object.regions.empty()) {
        Json::Value regionNodes;
        for (auto& region : object.regions) {
            regionNodes.append(toJson(region));
        }
        node["regions"] = regionNodes;
    }

	if (object.blocks) {
		node["blocks"] = toJson(*object.blocks, object.isTile() ? numeric_limits<int>::min() : object.y);
	}
    return node;
}

static Json::Value toJson(const ObjectGroup& objectGroup) {
    Json::Value node, objectNodes(Json::arrayValue);
    for (auto& object : objectGroup.objects) {
        objectNodes.append(toJson(object));
    }
    node["objects"] = objectNodes;
    return node;
}

void saveObjectGroup(const ObjectGroup& objectGroup, ostream& out) {
    Json::StyledStreamWriter writer(" ");
    writer.write(out, toJson(objectGroup));
}

/// LOAD ///

static BlockPos parseBlockPos(const Json::Value& node) {
    return BlockPos(node[0].asInt(), node[1].asInt(), node[2].asInt());
}

static BlockCuboid parseBounds(const Json::Value& posNode, const Json::Value& sizeNode) {
    return BlockCuboid::fromPositionAndSize(parseBlockPos(posNode), parseBlockPos(sizeNode));
}

static Region parseRegion(const Json::Value& node) {
	const auto parsedType = regiontype::fromString(node["type"].asString());
	return Region {
		node["name"].asString(),
		parseBounds(node["pos"], node["size"]),
		parsedType ? *parsedType : regiontype::Spawn,
		node["tags"].asString()
	};
}

static Door parseDoor(const Json::Value& node) {
	Door door;
    door.name = node["name"].asString();
	door.tags = node["tags"].asString();
	door.bounds = parseBounds(node["pos"], node["size"]);
    return door;
}

static Boundary parseBoundary(const Json::Value& node) {
	return {
		{ node[0].asInt(), node[1].asInt(), node[2].asInt() },
		node.size() == 4 ? node[3].asInt() : node[4].asInt() - node[1].asInt()
	};
}

static void parseRegions(const Json::Value& node, vector<Region>& regions) {
	for (const Json::Value& regionNode : node) {
        regions.push_back(parseRegion(regionNode));
    }
}

static void parseDoors(const Json::Value& node, vector<Door>& doors) {
	for (const Json::Value& doorNode : node) {
        doors.push_back(parseDoor(doorNode));
    }
}

static vector<Boundary> parseBoundaries(const Json::Value& maybeEncoded) {
	vector<Boundary> boundaries;

	if (maybeEncoded.isArray()) {
		for (const Json::Value& boundaryNode : maybeEncoded) {
			boundaries.push_back(parseBoundary(boundaryNode));
		}
	} else {
		string compressedBoundaries = Util::base64_decode(maybeEncoded.asString());
		string rawBoundaries;
		mCompressor->decompress(compressedBoundaries, rawBoundaries);
		
		{
			const int valuesPerBoundary { 4 };
			const int bytesPerValue { 2 };

			const auto getTwoByteInt = [](const char* s) {
				const uint8_t high = s[0];
				const uint8_t low = s[1];
				return static_cast<int>(high) << 8 | static_cast<int>(low);
			};

			for (int i = 0; i < rawBoundaries.size(); i += valuesPerBoundary * bytesPerValue) {
				boundaries.push_back({
					{
						getTwoByteInt(&rawBoundaries[i + 0]),
						getTwoByteInt(&rawBoundaries[i + 2]),
						getTwoByteInt(&rawBoundaries[i + 4])
					},
					getTwoByteInt(&rawBoundaries[i + 6])
				});
			}			
		}	
	}

	return boundaries;
}

static vector<uint8_t> parseRegionPlane(const string& encodedRegionPlane) {
	string compressedBlocks = Util::base64_decode(encodedRegionPlane);
	string rawBlocks;
	mCompressor->decompress(compressedBlocks, rawBlocks);
	return vector<uint8_t>(rawBlocks.begin(), rawBlocks.end());
}

static vector<WalkableHeight> parseWalkablePlane(const string& encodedWalkablePlane) {
	return algo::map_vector(parseRegionPlane(encodedWalkablePlane), RETLAMBDA(WalkableHeight(it)));
}

static Unique<BlockRegion> parseBlocks(const string& encodedBlocks, const BlockPos& size) {

	const auto nBlocks = static_cast<unsigned int>(size.product());

	string compressedBlocks = Util::base64_decode(encodedBlocks);
	
	string rawBlocks;
	rawBlocks.reserve(nBlocks*2); //D11.SC Pre-allocate to prevent memory reallocs during decompress
	mCompressor->decompress(compressedBlocks, rawBlocks);
	
	auto blocks = make_unique<BlockRegion>(size);
	
	const auto dataSize = (nBlocks + 1) / 2;
	const auto bytesPerBlock = (rawBlocks.size() - dataSize) / nBlocks;
	const auto dataStart = nBlocks * bytesPerBlock;

	if (bytesPerBlock == 1) {
		for (unsigned int i = 0; i < nBlocks; ++i)
		{
			BlockID blockId = static_cast<BlockID>(static_cast<uint8_t>(rawBlocks[i]));

			auto dataByte = rawBlocks[dataStart + i / 2];
			dataByte = (i & 1) ? dataByte : dataByte >> 4;
			DataID blockData = static_cast<DataID>(dataByte & 0xF);

			blocks->setBlock(i, FullBlock(blockId, blockData));
		}
	}
	else if (bytesPerBlock == 2) {
		for (unsigned int i = 0, bi = 0; i < nBlocks; ++i,  bi += 2)
		{			
			BlockID blockId = static_cast<BlockID>(static_cast<uint8_t>(rawBlocks[bi]) << 8 | static_cast<uint8_t>(rawBlocks[bi + 1]));

			auto dataByte = static_cast<uint8_t>(rawBlocks[dataStart + bi / 4]);
			dataByte = (bi & 2) ? dataByte : dataByte >> 4;
			DataID blockData = static_cast<DataID>(dataByte & 0xF);

			blocks->setBlock(i, FullBlock(blockId, blockData));
		}
	}
	else {
		checkNoEntry();
	}

	return blocks;
}

static Object parseObject(const Json::Value& node) {
	
    Object object;
    object.id = node["id"].asString();
	object.bounds = parseBounds(node["pos"], node["size"]);
    object.y = node["y"].asInt();
	object.tags = node["tags"].asString();

	parseDoors(node["doors"], object.doors);
	parseRegions(node["regions"], object.regions);
	if (node.isMember("boundaries")) {
		object.boundaries = parseBoundaries(node["boundaries"]);
	}

	if (node.isMember("region-plane")) {
		object.regionPlane = parseRegionPlane(node["region-plane"].asString());
	}
	if (node.isMember("region-y-plane")) {
		object.maxHeightOfRegionTerrainPlane = parseRegionPlane(node["region-y-plane"].asString());
	}
	if (node.isMember("height-plane")) {
		object.heightPlane = parseRegionPlane(node["height-plane"].asString());
	}
	if (node.isMember("walkable-plane")) { // @todo: fix this!
		object.walkablePlaneFromLovika = parseWalkablePlane(node["walkable-plane"].asString());
	}
	if (!object.regionPlane.empty()) {
		object.walkablePlane = algo::map_vector(object.regionPlane, RETLAMBDA(TerrainCell(it).isReachable()? WalkableHeight::Min() : WalkableHeight::Unreachable() ));
	} else {
		object.walkablePlane = std::vector<WalkableHeight>(object.bounds.area(), WalkableHeight::Min());
	}

	if (node.isMember("blocks")) {
		object.blocks = parseBlocks(node["blocks"].asString(), object.bounds.size());
	}
    return object;
}

static Unique<ObjectGroup> parseObjectGroup(const Json::Value& node) {
    auto objectGroup = make_unique<ObjectGroup> ();

	{
		const Json::Value& ObjectsNode = node["objects"];

		const int NumObjects = ObjectsNode.size();
		int32 NumThreads = FPlatformProcess::SupportsMultithreading() ? FPlatformMisc::NumberOfCores() : 1;
		int32 NumObjectsPerThreadMax = 1;
		int32 NumObjectsPerThreadMin = 1;
		int32 LastThreadMax = NumThreads;

		if (NumObjects > NumThreads)
		{
			const int32 ObjectsRemainder = NumObjects % NumThreads;
		
			if (ObjectsRemainder == 0)
			{
				NumObjectsPerThreadMin = NumObjectsPerThreadMax = NumObjects / NumThreads;
			}
			else
			{
				NumObjectsPerThreadMin = NumObjects / NumThreads;
				NumObjectsPerThreadMax = NumObjectsPerThreadMin + 1;
				LastThreadMax = ObjectsRemainder;
			}
		}
		else
		{
			NumThreads = NumObjects;
			LastThreadMax = NumThreads;
		}
		
		//pre-allocate all objects in array
		objectGroup->objects.reserve(NumObjects);
		objectGroup->objects.resize(NumObjects);

		ParallelFor(NumThreads, [NumObjectsPerThreadMax, NumObjectsPerThreadMin, LastThreadMax, &ObjectsNode, &objectGroup](int32 threadIndex)
		{
			const int LowCount = (threadIndex < LastThreadMax) ?  0 : 1;
			const int startIndex = (LowCount) ? ((LastThreadMax * NumObjectsPerThreadMax) + ((threadIndex - LastThreadMax) * NumObjectsPerThreadMin)) : (threadIndex * NumObjectsPerThreadMax);

			int EndIndex = startIndex + ((LowCount * NumObjectsPerThreadMin) + ((1 - LowCount) * NumObjectsPerThreadMax));

			for (int i(startIndex); i < EndIndex; ++i)
			{
				const Json::Value& objectNode = ObjectsNode[i];
				objectGroup->objects[i] = parseObject(objectNode);
			}
		});

	}

	
    return objectGroup;
}

Unique<ObjectGroup> loadObjectGroup(istream& in, const std::string& path) {
    Json::Value root;
    Json::Reader reader;
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_levelgen_sourcedata_loadObjectGroup_reader_parse);
		if (!reader.parse(in, root)) {
			cout << "Failed to parse objectgroup\n" << reader.getFormattedErrorMessages();
			return nullptr;
		}
	}

	auto objectGroup = parseObjectGroup(root);
	if (objectGroup) {
		objectGroup->def = path;
	}

	return objectGroup;
}

Unique<io::ObjectGroup> loadObjectGroup(TArray<uint8>& src, const std::string& path)
{
	Json::Value root;
	Json::Reader reader;
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_levelgen_sourcedata_loadObjectGroup_reader_parse);
		if (!reader.parse((char*)src.GetData(), ((char*)src.GetData()) + src.Num(), root)) {
			cout << "Failed to parse objectgroup\n" << reader.getFormattedErrorMessages();
			return nullptr;
		}
	}

	src.Empty(); //D11.SC dont need this memory once its been parsed by the json reader

	auto objectGroup = parseObjectGroup(root);	
	if (objectGroup) {
		objectGroup->def = path;
	}

	return objectGroup;
}

}
