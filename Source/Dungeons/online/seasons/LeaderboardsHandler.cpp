#include "LeaderboardsHandler.h"
#include "modules/LiveOps/LeaderboardResponse.h"
#include "util/StringUtil.h"
#include "core/HttpServiceResponse.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"
#include "Engine/GameInstance.h"
namespace minecraft {
namespace api {
	LeaderboardResponse GenerateLeaderboardsTestResponse() {
		LeaderboardResponseEntry e1 { 1, 10, "Jonas" };
		LeaderboardResponseEntry e2 { 2, 5, "Kristoffer" };
		LeaderboardResponseEntry e3 { 1, 100, "Kristoffer" };
		LeaderboardResponseEntry e4 { 2, 50, "Jonas" };

		LeaderboardResponseList rl1;
		rl1.List->push_back(e1);
		rl1.List->push_back(e2);

		LeaderboardResponseList rl2;
		rl2.List->push_back(e3);
		rl2.List->push_back(e4);

		LeaderboardResponse r;
		r.Map->insert({"XP", std::move(rl1)});
		r.Map->insert({"Kills", std::move(rl2)});
		return r;
	}
}
}

namespace online
{
namespace liveops
{

	void LeaderboardsHandler::Teardown()
	{
		mBoards.ClearLeaderboards();
	}

void LeaderboardsHandler::Request() {
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void LeaderboardsHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) {
	const auto callback = [this, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::LeaderboardResponse> response) {
		//TODO: Remove this once we have real data for the response from the back-end
		const auto mockData = minecraft::api::GenerateLeaderboardsTestResponse();
		Leaderboards newLeaderboards{};

		for (const auto& key : *mockData.Map) {
			std::string keyString = key.first;
			std::vector<LeaderboardEntry> targetLeaderboardList{};
			const auto& sourceLeaderboard = key.second.List;
			targetLeaderboardList.reserve(sourceLeaderboard->size());
			for (const auto& elem : *sourceLeaderboard) {
				LeaderboardEntry le{ *elem.mRank, *elem.mValue, *elem.mName };
				targetLeaderboardList.push_back(std::move(le));
			}
			newLeaderboards.AddLeaderboard(std::move(keyString), std::move(targetLeaderboardList));
		}

		SetLeaderboards(std::move(newLeaderboards));

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
}

Leaderboards LeaderboardsHandler::GetLeaderboards() const{
	return mBoards;
}

void LeaderboardsHandler::SetLeaderboards(Leaderboards&& lb) {
	mBoards = std::move(lb);
}

}
}
