#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "GameplayEffectTypes.h"
#include "world/entity/EntityTypes.h"
#include "game/levels.h"
#include "game/affector/Affectors.h"
#include "game/mission/variation/LevelVariationType.h"

struct FMissionFinishedSummary;
class UGameInstance;
class ItemType;

struct DailyTrialFilterData {
	TOptional<FString> TrialId;
	TOptional<FString> Type;
};

namespace online
{
namespace liveops
{

struct FilterPart;

namespace filter
{
namespace killobjective
{
	bool isMatch(const std::vector<FilterPart>& filter, EntityType);
}
namespace missioncompleted
{
	bool isMatch(const std::vector<FilterPart>& filter, ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, const DailyTrialFilterData&);
}
namespace healingdone
{
	bool isMatch(const std::vector<FilterPart>& filter, const FOnAttributeChangeData&);
}
namespace revivefriend 
{
	bool isMatch(const std::vector<FilterPart>& filter);
}
namespace currencycollected
{
	bool isMatch(const std::vector<FilterPart>& filter, UGameInstance* GameInstance, const FName&, const int32&);
}
namespace itemuse
{
	bool isMatch(const std::vector<FilterPart>& filter, const ItemType&);
}
}
}
}
