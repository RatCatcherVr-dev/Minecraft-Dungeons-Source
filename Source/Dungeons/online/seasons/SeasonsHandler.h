#pragma once

#include <string>
#include <memory>
#include <vector>

#include "modules/LiveOps/SeasonResponse.h"
#include "LiveIF.h"
#include "Rewards.h"
#include "core/HttpServiceResponse.h"
#include "Util/StringUtil.h"
#include "online/seasons/Util/SeasonsCommon.h"

namespace online
{
namespace liveops
{

class SeasonBase {
public:
	explicit SeasonBase(minecraft::api::Season response)
		: Response(response)
		, StartTimeUTC(ToFDateTime(response.activeFrom))
	{}

	const std::string& GetName() const { return Response.name; }
	const std::string& GetAPProgressName() const { return Response.name; }
	const std::string& GetDisplayName() const { return Response.displayName; }
	const FDateTime& GetStartDateTimeUTC() const { return StartTimeUTC; }
	const std::vector<minecraft::api::SeasonRewardLevel>& GetRewardLevels() const { return Response.rewardLevels; }

protected:
	minecraft::api::Season Response;
	FDateTime StartTimeUTC;
};

class Season : public SeasonBase {
public:
	Season(int number, minecraft::api::Season response)
		: SeasonBase(std::move(response))
		, Number(number)
	{}

	int GetNumber() const { return Number; }

private:
	int Number;
};

class Event : public SeasonBase {
public:
	Event(minecraft::api::Season response)
		: SeasonBase(std::move(response))
		, EndTimeUTC(ToFDateTime(Response.activeTo.GetValue()))
	{}

	const FDateTime& GetEndDateTimeUTC() const { return EndTimeUTC; }

private:
	FDateTime EndTimeUTC;
};

class SeasonsHandler : public LiveIF {
public:
	SeasonsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* gameInstance);

	virtual void Init() override;
	virtual void Teardown() override;
	virtual void Request() override;
	virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;

	const std::vector<Season>& GetSeasons() const;
	TOptional<Event> GetCurrentEvent() const;

private:
	void UpdateSeasons(minecraft::api::HttpServiceResponse<minecraft::api::SeasonResponse>& response);
	void UpdateEvents(minecraft::api::HttpServiceResponse<minecraft::api::SeasonResponse>& response);

	std::vector<Season> Seasons;
	std::vector<Event> Events;
};

}
}
