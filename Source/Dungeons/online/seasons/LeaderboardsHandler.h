#pragma once
#include "modules/LiveOps/LeaderboardResponse.h"
#include <iterator>
#include "core/HttpServiceResponse.h"
#include "online/sessions/OnlineUtil.h"
#include "Engine/GameInstance.h"
#include <vector>
#include "IDungeonsAPIClient.h"
#include "Util/SeasonsCommon.h"
#include "LiveIF.h"

namespace online
{
namespace liveops {

	struct LeaderboardEntry {
		int64 mRank;
		int64 mValue;
		std::string mName;
	};
	
	struct Leaderboards {
	private:
		std::unordered_map<std::string, std::vector<LeaderboardEntry>> mLeaderboards;
	public:
		const std::unordered_map<std::string, std::vector<LeaderboardEntry>>& GetLeaderboardMap() {
			return mLeaderboards;
		}

		void AddLeaderboard(const std::string& key, std::vector<LeaderboardEntry>&& lb) {
			mLeaderboards.emplace(key,lb);
		}
		void ClearLeaderboards() {
			mLeaderboards.clear();
		}
	};

	class LeaderboardsHandler : public LiveIF {
		Leaderboards mBoards;
	public:
		LeaderboardsHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* InGameInstance) : LiveIF(apiClient, InGameInstance) {}

		virtual void Teardown() override;
		virtual void Init() override {}
		virtual void Request() override;
		virtual void MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) override;

		Leaderboards GetLeaderboards() const;
		void SetLeaderboards(Leaderboards&& lb);
	};
}
}

