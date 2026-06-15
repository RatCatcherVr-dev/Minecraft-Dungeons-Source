#pragma once
#include "common_header.h"

#include "world/level/chunk/LevelChunkFormat.h"
#include "world/level/chunk/SubLevelChunkFormat.h"

namespace Common {
	std::string getGameVersionString();
	std::string getGameVersionStringNet();
	std::string getGameDevVersionString();
	FString getStoreName();
}

namespace SharedConstants {
	//TODO move this to another place, doesn't seem like the right one
	constexpr int TicksPerSecond = 20;
	constexpr float SecondsPerTick = 1.f / 20.f;
	constexpr int TicksPerMinute = TicksPerSecond * 60;
	constexpr int TotalTrialTicks = TicksPerMinute * 90;

	// v[MajorVersuion].[MinorVersion].[PatchVersion].[RevisionVersion]
	extern const int MajorVersion;
	extern const int MinorVersion;
	extern const int PatchVersion;
	extern const int RevisionVersion;
	extern const int IsBeta;

	extern const int NetworkProtocolVersion;

	extern const int AutomationProtocolVersion;

	extern const int CompanionAppProtocolVersion;

	extern const int MaxChatLength;

	extern const int LevelDBCompressorID;

	extern const LevelChunkFormat CurrentLevelChunkFormat;
	extern const SubLevelChunkFormat CurrentSubLevelChunkFormat;

	extern const uint16_t NetworkDefaultGamePort;
	extern const uint16_t NetworkDefaultGamePortv6;
	extern const int NetworkDefaultMaxPlayers;

	int getVersionCode();
	int getVersionCode(int major, int minor, int patch, int beta = 0);
	bool isVersion(int major, int minor, int patch, int beta = 0);
}

const int INVENTORY_MAX_SELECTION_SIZE = 9;
const int SURVIVAL_INVENTORY_SIZE = 36;	//slots added after the first INVENTORY_MAX_SELECTION_SIZE slots
const int CREATIVE_INVENTORY_SIZE = 9;	//extra slots added after regular inventory
