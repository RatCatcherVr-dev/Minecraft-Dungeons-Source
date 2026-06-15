#pragma once

#include <Map.h>
#include <Array.h>
#include <UnrealString.h>

enum class EAwardType : uint8;

namespace awards
{
	struct AwardData;
}

namespace analytics
{
	typedef std::map<std::string, std::string> PropertyMap;

	///////////////////// // // //  HELPERS // // // /////////////////////////////

	struct AwardData
	{
		FString typeName;
		EAwardType type;
		int32 score;
		FString playerName;
		int playerNumber;

		AwardData(const FString& _typeString, EAwardType _type, int32 _score, const FString& _playerName, int _playerNumber);
		AwardData(const awards::AwardData& data);
	};

	struct PlayerStats
	{
		TMap<FString, int> MobKills;
		TMap<FString, int> ItemUses;
		TMap<FString, int> ScalarStats;

		// additions: log XP gain/loss ... 
	};
}

