#include "Dungeons.h"
#include "SourceData.h"
#include "game/util/UnlockKeyUtils.h"
#include "game/mission/MissionDef.h"
#include "lovika/io/TileIo.h"
#include "lovika/tile/TilePredicates.h"
#include "util/Algo.hpp"
#include "util/StringUtil.h"
#include <LogMacros.h>

namespace levelgen { namespace sourcedata {

enum class FileType {
	ObjectGroup,
	LevelFile,
	HyperLevelFile,
	MobGroups,
	Objectives,
};

SourceData::SourceData(Prefabs prefabs, io::Level level)
	: tiles(prefabs.tileSet.createGroup(level.tileDefs))
	, props(prefabs.propSet.createGroup(level.propDefs))
	, level(std::move(level))
	, prefabs(std::move(prefabs)) {
}

Prefabs&& SourceData::stealPrefabs() {
	return std::move(prefabs);
}


static FString getJsonData(FileType fileType, FString fn) {
	const FString basePath = FString("Content/data/lovika") / (fileType == FileType::ObjectGroup ? "objectgroups" : "levels");
	const FString filename = [&] {
		if (fileType == FileType::HyperLevelFile) return fn + "-hyper";
		if (fileType == FileType::Objectives) return fn + ".objectives";
		return fn;
	}();
	const FString pathAndFilename = FPaths::ProjectDir() / basePath / filename + ".json";
	FString contents;
	return FFileHelper::LoadFileToString(contents, *pathAndFilename) ? contents : "";
}

static bool getJsonData(FileType fileType, FString fn, TArray<uint8>& result) {
	const FString basePath = FString("Content/data/lovika") / (fileType == FileType::ObjectGroup ? "objectgroups" : "levels");
	const FString filename = [&] {
		if (fileType == FileType::HyperLevelFile) return fn + "-hyper";
		if (fileType == FileType::Objectives) return fn + ".objectives";
		return fn;
	}();
	const FString pathAndFilename = FPaths::ProjectDir() / basePath / filename + ".json";
	const bool succeed = FFileHelper::LoadFileToArray(result, *pathAndFilename);
	result.Add(0);//null terminate(FFileHelper::LoadFileToArray adds 2 bytes for this purpose)
	return succeed;
}

bool fillFromFileSystem(TileSet& prefabSet, const FString& groupName) {
	return fillFromFileSystem(prefabSet, prefabSet, groupName);
}

bool fillFromFileSystem(TileSet& tileSet, TileSet& propSet, const FString& groupName) {
	TArray<uint8> groupContents;
	
	if (!getJsonData(FileType::ObjectGroup, groupName, groupContents)) {
		return false;
	}

	auto objectGroup = io::loadObjectGroup(groupContents, stringutil::toStdString(groupName));

	if (!objectGroup) {
		return false;
	}
	auto objects = io::tile::fromObjectGroup(*objectGroup);
	tileSet.feed(objects.tiles);
	propSet.feed(objects.props);
	return true;
}

bool loadMobGroups(io::Level& level, const FString& jsonData) {
	if (!jsonData.IsEmpty()) {
		std::istringstream in(TCHAR_TO_UTF8(*jsonData));
		return io::loadMobGroups(level, in);
	}
	return true;
}

bool loadMobGroups(io::Level& level, const std::vector<CaseInsensitiveId>& mobGroupLevelIds) {
	bool err = loadMobGroups(level, getJsonData(FileType::MobGroups, "mobgroups"));
	for (auto&& group : algo::unique_by_less_of(mobGroupLevelIds, RETLAMBDA(it.lowerId))) {
		err = loadMobGroups(level, getJsonData(FileType::MobGroups, FString(group.id.c_str()))) && err;
	}
	return err;
}

bool loadObjectives(io::Level& level, const FString& levelName) {
	auto data = getJsonData(FileType::Objectives, levelName);
	if (!data.IsEmpty()) {
		std::istringstream in(TCHAR_TO_UTF8(*data));
		return io::loadObjectives(level, in);
	}
	return true;
}

Unique<SourceData> fromFileSystem(const FString& levelName) {
	auto level = make_unique<io::Level>();
	auto levelData = getJsonData(FileType::LevelFile, levelName);
	auto levelDataUtf = std::string(TCHAR_TO_UTF8(*levelData));

	std::istringstream inHeader(levelDataUtf);
	auto header = io::loadLevelHeader(inHeader);
	if (!header) {
		DEBUG_FAIL("Couldn't load level file (header). Invalid json?");
		return {};
	}

	if (!loadMobGroups(*level, header->mobGroupFileIds)) {
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load mobgroups file."));
	}

	std::istringstream inLevel(levelDataUtf);
	if (!io::loadLevel(*level, inLevel)) {
		DEBUG_FAIL("Couldn't load level file. Invalid json?");
		return {};
	}

	if (!loadObjectives(*level, levelName)) {
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load %s.objectives.json file."), *levelName);
	}

	Prefabs prefabs;
	checkf(!level->objectGroups.empty(), TEXT("Must have at least one <object-group>"));
	for (const auto& objectGroup : algo::unique_by_less_of(level->objectGroups, RETLAMBDA(it.path.lowerId))) {
		fillFromFileSystem(prefabs.tileSet, prefabs.propSet, FString(objectGroup.path.id.c_str()));
	}
	if (prefabs.tileSet.isEmpty()) {
		return {};
	}
	return std::make_unique<SourceData>(std::move(prefabs), *level);
}

Unique<io::HyperLevel> hyperLevelFromFileSystem(const FString& levelName) {
	const auto levelDataUtf = stringutil::toStdString(getJsonData(FileType::HyperLevelFile, levelName));
	std::istringstream is(levelDataUtf);
	return io::loadHyperLevel(is);
}

}}
