#include "Dungeons.h"
#include "AnalyticsDataTypes.h"

#include "util/DungeonsAwardGenerator.h"
#include "util/DungeonsPlayerStatTracker.h"

namespace analytics
{
	AwardData::AwardData(const FString& _typeName, EAwardType _type, int32 _score, const FString& _playerName, int _playerNumber)
		: typeName(_typeName)
		, type(_type)
		, score(_score)
		, playerName(_playerName)
		, playerNumber(_playerNumber)
	{}

	AwardData::AwardData(const awards::AwardData& data)
		: type(data.type)
		, score(data.score)
		, playerNumber(data.playerNumber)
		, playerName("")
		, typeName(awards::toString(data.type))
	{}
}

