#include "ChallengesHandler.h"
#include "core/HttpServiceResponse.h"
#include "Engine/GameInstance.h"
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "IHttpResponse.h"
#include "modules/LiveOps/ChallengeResponse.h"
#include "ObjectiveTypes/KillObjective.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"
#include "Util/Filter.h"

namespace minecraft {
	namespace api {
		using Challenges = std::vector<ChallengeResponse>;

		Challenges GenerateChallengesTestResponse() {
			ChallengeResponse spiderChallenge;
			spiderChallenge.name = "kill_spiders";
			spiderChallenge.activeFrom = "2021-01-01T00:00:00Z";
			spiderChallenge.activeTo = "2021-12-31T23:59:59:59Z";
			spiderChallenge.objectives = {
				{
					"kill_spiders_progress_key",
					ObjectiveType::MobDeath, {
						{FilterPartType::Mob, "Spider"}
					}, 20
				}
			};
			spiderChallenge.rewards = {
				{"Reward1"}
			};

			ChallengeResponse missionChallenge;
			missionChallenge.name = "complete_missions";
			missionChallenge.activeFrom = "2021-01-01T00:00:00Z";
			missionChallenge.activeTo = "2021-12-31T23:59:59:59Z";
			missionChallenge.objectives = {
				{
					"complete_missions_progress_key",
					ObjectiveType::MissionCompleted, {}, 10
				}
			};
			missionChallenge.rewards = {
				{"Reward2"}
			};

			ChallengeResponse potionsChallenge;
			potionsChallenge.name = "use_potions";
			potionsChallenge.activeFrom = "2021-01-01T00:00:00Z";
			potionsChallenge.activeTo = "2021-12-31T23:59:59:59Z";
			potionsChallenge.objectives = {
				{
					"use_potions_progress_key",
					ObjectiveType::ItemUsed, {
						{FilterPartType::ItemTag, "Potion"},
						{FilterPartType::Operator, "OR"},
						{FilterPartType::ItemTag, "HealthPotion"}
					}, 50
				}
			};
			potionsChallenge.rewards = {
				{"Reward3"}
			};

			ChallengeResponse travelChallenge;
			travelChallenge.name = "travel";
			travelChallenge.activeFrom = "2021-01-01T00:00:00Z";
			travelChallenge.activeTo = "2021-12-31T23:59:59:59Z";
			travelChallenge.objectives = {
				{
					"travel_progress_key",
					ObjectiveType::BlocksTravelled, {
					}, 15000
				}
			};
			travelChallenge.rewards = {
				{"Reward4"}
			};

			ChallengeResponse chestChallenge;
			chestChallenge.name = "open_chests";
			chestChallenge.activeFrom = "2021-01-01T00:00:00Z";
			chestChallenge.activeTo = "2021-12-31T23:59:59:59Z";
			chestChallenge.objectives = {
				{
					"open_chests_progress_key",
					ObjectiveType::ChestOpened, {
					}, 40
				}
			};
			chestChallenge.rewards = {
				{"Reward5"}
			};

			return { spiderChallenge, missionChallenge, potionsChallenge, travelChallenge, chestChallenge };
		}
	}
}

namespace online {
namespace liveops {


ChallengesHandler::ChallengesHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* InGameInstance) :
	LiveIF(apiClient, InGameInstance), useMockedChallenges(false) {
}

void ChallengesHandler::Teardown() {
	challenges.challenges.clear();
}

void ChallengesHandler::Init() {
}

void ChallengesHandler::Request() {
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void ChallengesHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) {
	if (ShouldUseMockedChallenges()) {
		InternalUpdateChallenges(minecraft::api::GenerateChallengesTestResponse());

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(minecraft::api::HttpServiceResponse<minecraft::api::ChallengeResponse>(EHttpResponseCodes::Ok));
		}

		return;
	}

	LiveOpsClient::Callback<minecraft::api::ChallengesResponse> callback = [&, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::ChallengesResponse> response) {
		if (response.successful()) {
			InternalUpdateChallenges(response.getBody()->challenges);
		} else {
			UE_LOG(LogLiveOps, Error, TEXT("Unsuccessful http response for challenges"));
		}

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
	mApiClient->Request<DungeonsEndpoint::CHALLENGES>(callback);
}

void ChallengesHandler::UseMockedChallenges() {
	useMockedChallenges = true;
	Request();
}

bool ChallengesHandler::ShouldUseMockedChallenges() const {
#if UE_BUILD_SHIPPING
	return false;
#else
	return useMockedChallenges;
#endif
}

const Challenges& ChallengesHandler::GetChallenges() const {
	return challenges;
}

Challenge* ChallengesHandler::GetChallenge(const std::string& name) const {
	for (const auto& challenge : challenges.challenges) {
		if (challenge->GetName() == name) {
			return challenge.get();
		}
	}
	return nullptr;
}

void ChallengesHandler::InternalUpdateChallenges(const std::vector<minecraft::api::ChallengeResponse>& challengesResponse) {
	challenges.challenges.clear();
	for (const auto& challenge : challengesResponse) {
		challenges.Add(ChallengeConfig{ GameInstance, mApiClient }, challenge);
	}
}
}
}
