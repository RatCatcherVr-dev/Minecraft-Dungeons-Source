#pragma once

#include "IoMobTypes.h"

namespace io {

using RegionLocator = std::string;

struct ObjectiveMobs {
	int count = 1;
	std::vector<MobGroup> groups;
	TOptional<std::string> spawnAtRegion;
	bool showSpawnIndicator = true;
};


struct ObjectiveGate {
	enum Type {
		Nothing     = 0x0,
		Gates       = 0x1,
		TileExits   = 0x2,
		Regions	    = 0x4
	};
	TOptional<int> type;
	std::vector<RegionLocator> regions;
	TOptional<FString> object;
	TOptional<bool> startUnlocked;
};

struct ObjectiveGauntletData {
	RegionLocator endRegion;
	ObjectiveGate gate;
	TOptional<RegionLocator> markerRegion;
};

struct ObjectiveKillGroupData {
	ObjectiveMobs mobs;
	std::string stretch;
	int amount;
	bool showCount;
	std::string rewardId;
	ObjectiveGate gate;
	std::vector<RegionLocator> spawnRegions;
	TOptional<RegionLocator> markerRegion;
};

struct ObjectiveSoundData {
	std::string object;
};

struct ObjectiveArenaBattleData {
	float startTime;
	float waveInterval;
	float restingTime;
	float preSpawnMobDuration;
	float battleMaxDuration;
	bool calculateTimeFromWaveStart;
	bool isBoss = false;
	bool hasMusic = true;
	std::vector<ObjectiveMobs> waves;
	std::vector<EntityType> mobActivations;
	std::string stretch;
	std::string spawnEffectsPack;
	TOptional<std::string> preSpawnMobObject;
	ObjectiveGate gate;
	TOptional<float> endDelaySeconds;
};

struct ObjectiveClickData {
	int count;
	std::string object;
	TOptional<FString> dummyObject;
	bool isSequential = false;
	//std::vector<Rotations> rotations;
	std::vector<RegionLocator> locations;
	std::vector<RegionLocator> lockedDoors;
	std::vector<RegionLocator> keyLocations;
	TOptional<std::string> keyType;
	TOptional<ObjectiveMobs> mobs;
	TOptional<std::string> mobStretch;
	TOptional<std::vector<RegionLocator>> spawnRegions;
	TOptional<FString> doorPath;
};

enum class ObjectiveType { None, Arena, Click, Gauntlet, KillGroup, };

enum class ObjectiveConsiderTilesType { Main, All, AlwaysVisible, /* following are only for challenges */ Tile, Stretch };

struct ObjectiveInfo {
	FText name;
	FText description;
	FString rawName;
	FString rawDescription;
	std::vector<std::string> meta;
	TOptional<FString> id;
	TOptional<ObjectiveConsiderTilesType> considerTiles;
	std::vector<RegionLocator> requiredRegions;

	TOptional<FString> icon;
	FString displayMode = "SubObjective";
	bool isHidden = false;
	bool alwaysShow = false;
	bool isOptional = false;
	bool panToObjectiveStart = true;
	bool panToObjectiveEnd = true;
	bool forceObjectiveIndicator = false;
	float panToDuration = 2.0f;
	int objectiveIndex = -1;
	int objectiveCount = -1;

	ObjectiveType type;
};

struct ObjectiveSoundInfo {
	bool triggerMusicPlaybackEvents = false; 
	TOptional<ObjectiveSoundData> track;
	TOptional<ObjectiveSoundData> completedTrack;
	TOptional<ObjectiveSoundData> narrator;
};

struct Objective {
	ObjectiveInfo info;
	ObjectiveSoundInfo sound;
	TOptional<ObjectiveGauntletData> gauntlet;
	TOptional<ObjectiveGauntletData> hidden;
	TOptional<ObjectiveArenaBattleData> arena;
	TOptional<ObjectiveClickData> click;
	TOptional<ObjectiveKillGroupData> killGroup;

	ObjectiveType type;
};

// @note @attn: the contents of this struct will change (how to decide type, and amount of it (e.g. emeralds))
struct ObjectiveReward {
	TOptional<RegionLocator> locator;
	TOptional<FString> customPrefab;
};

struct Challenge {
	Objective objective;
	ObjectiveReward reward;
	RegionLocator trigger;
	TOptional<FString> triggerObject;
};

}
