#pragma once

#include "CommonTypes.h"
#include "TileSet.h"
#include "lovika/io/LevelFile.h"

struct MissionDef;

namespace levelgen { namespace sourcedata {

struct Prefabs {
	TileSet tileSet;
	TileSet propSet;
};

struct SourceData {
	SourceData(Prefabs, io::Level);

	TileGroup tiles;
	TileGroup props;
	io::Level level;

	Prefabs&& stealPrefabs();
private:
	Prefabs prefabs;
};

bool fillFromFileSystem(TileSet& prefabSet, const FString& groupName);
bool fillFromFileSystem(TileSet& tileSet, TileSet& propSet, const FString& groupName);
Unique<SourceData> fromFileSystem(const FString& levelName);

Unique<io::HyperLevel> hyperLevelFromFileSystem(const FString& levelName);

}}
