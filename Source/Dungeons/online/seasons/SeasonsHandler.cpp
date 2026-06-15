#include "SeasonsHandler.h"

#include <algorithm>

#include "AdventurePointsHandler.h"
#include "IDungeonsAPIClient.h"
#include "RewardsHandler.h"
#include "core/HttpServiceResponse.h"
#include "Util/SeasonsCommon.h"
#include "online/sessions/OnlineUtil.h"
#include "util/DateTimeUtil.h"

namespace online
{
namespace liveops
{

namespace internal
{

std::vector<minecraft::api::Season> GetSeasonsWithType(minecraft::api::SeasonType type, const std::vector<minecraft::api::Season>& seasons)
{
	std::vector<minecraft::api::Season> result;
	result.reserve(seasons.size());
	std::copy_if(seasons.begin(), seasons.end(), std::back_inserter(result), [type](const auto& season) { return season.type == type; });
	return result;
}

void OrderByAscendingStartDate(std::vector<minecraft::api::Season>& seasons)
{
	std::sort(seasons.begin(), seasons.end(),
		[](const auto& lhs, const auto& rhs) {
			return ToFDateTime(lhs.activeFrom) < ToFDateTime(rhs.activeFrom);
		});
}

bool EventIsActive(const Event& event)
{
	return DateTimeUtil::InDateTimeWindow(event.GetStartDateTimeUTC(), event.GetEndDateTimeUTC());
}

}

SeasonsHandler::SeasonsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* gameInstance)
	: LiveIF(apiClient, gameInstance)
{
}

void SeasonsHandler::Init()
{
}

void SeasonsHandler::Teardown()
{
}

void SeasonsHandler::Request()
{
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void SeasonsHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest)
{
	LiveOpsClient::Callback<minecraft::api::SeasonResponse> callback = [&, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::SeasonResponse> response) {
		if (response.successful()) {
			UpdateSeasons(response);
			UpdateEvents(response);
		}
		else {
			UE_LOG(LogLiveOps, Error, TEXT("Unsuccessful http response for SeasonsHandler"));
		}

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
	mApiClient->Request<DungeonsEndpoint::CONFIG_SEASONS>(callback);
}

const std::vector<Season>& SeasonsHandler::GetSeasons() const
{
	return Seasons;
}

TOptional<Event> SeasonsHandler::GetCurrentEvent() const
{
	check(std::count_if(Events.begin(), Events.end(), internal::EventIsActive) <= 1 && "Found more than one active event");

	for (const auto& event : Events) {
		if (internal::EventIsActive(event)) {
			return event;
		}
	}

	return {};
}

void SeasonsHandler::UpdateSeasons(minecraft::api::HttpServiceResponse<minecraft::api::SeasonResponse>& response)
{
	auto newSeasons = internal::GetSeasonsWithType(minecraft::api::SeasonType::Endless, response.getBody()->seasons);
	internal::OrderByAscendingStartDate(newSeasons);

	Seasons.clear();
	for (int i = 0; i < newSeasons.size(); ++i) {
		Seasons.emplace_back(i + 1, newSeasons[i]);
	}
}

void SeasonsHandler::UpdateEvents(minecraft::api::HttpServiceResponse<minecraft::api::SeasonResponse>& response)
{
	auto newEvents = internal::GetSeasonsWithType(minecraft::api::SeasonType::Event, response.getBody()->seasons);
	internal::OrderByAscendingStartDate(newEvents);

	Events.clear();
	for (const auto& event : newEvents) {
		Events.emplace_back(event);
	}
}

}
}
