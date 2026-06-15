#pragma once
#include <memory>

#include "Ticker.h"
#include <memory>

#include "Util/SeasonsCommon.h"
#include "SeasonsHandler.h"
#include "Rewards.h"
#include "RewardsHandler.h"
#include "ChallengesHandler.h"
#include "ChallengesView.h"
#include "EventView.h"
#include "LeaderboardsHandler.h"
#include "ProgressHandler.h"
#include "AdventurePointsHandler.h"
#include "SeasonView.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "Util/PollBase.h"

class UGameInstance;

namespace online {
namespace liveops {

DECLARE_MULTICAST_DELEGATE_OneParam(FLiveOpsOnDataUpdate, UpdateRequestStatus);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLiveOpsOnSeasonViewUpdate, UpdateRequestStatus, const SeasonView&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLiveOpsOnChallengesViewUpdate, UpdateRequestStatus, const ChallengesView&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLiveOpsOnEventViewUpdate, UpdateRequestStatus, const TOptional<EventView>&);

DECLARE_MULTICAST_DELEGATE_EightParams(FOnMissionFinished, ELevelNames, EGameDifficulty, FEndlessStruggle, EExtraChallenge, bool, ELevelVariationType, int, TOptional<FString>);
DECLARE_MULTICAST_DELEGATE_OneParam(FLiveOpsOnKilledOther, const EntityType);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXPChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FLiveOpsOnItemUsed, const ItemType&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLiveOpsOnHealed, float, const FOnAttributeChangeData&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FLiveOpsOnMovement, float /* DeltaSeconds */, const FVector& /* oldLocation */, const FVector& /* oldVelocity */);
DECLARE_MULTICAST_DELEGATE(FLiveOpsOnRevive);
DECLARE_MULTICAST_DELEGATE(FLiveOpsOnOpenChest);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, const FName&, int32);


class LiveOps {
public:
	LiveOps(UGameInstance*);
	~LiveOps();

	LeaderboardsHandler* GetLeaderboards() const;
	ChallengesHandler* GetChallenges() const;
	SeasonsHandler* GetSeasonHandler() const;
	ProgressHandler* GetProgressHandler() const;
	AdventurePointsHandler* GetAdventurePointsHandler() const;
	RewardsHandler* GetRewardsHandler() const;

	void RequestDataUpdateAsync() const;
	void RequestSeasonViewUpdateAsync() const;
	void RequestChallengesViewUpdateAsync() const;
	void RequestEventViewUpdateAsync() const;
	SeasonView GetSeasonView() const;
	ChallengesView GetChallengesView() const;
	TOptional<EventView> GetEventView() const;

	EMinecraftAPIConnectionStatus GetConnectionStatus() const;

	FLiveOpsOnDataUpdate OnDataUpdate;
	FLiveOpsOnSeasonViewUpdate OnSeasonViewUpdate;
	FLiveOpsOnChallengesViewUpdate OnChallengesViewUpdate;
	FLiveOpsOnEventViewUpdate OnEventViewUpdate;

	FOnMissionFinished OnMissionFinished;
	FLiveOpsOnKilledOther OnKilledOther;
	FOnXPChanged OnXPChanged;
	FLiveOpsOnHealed OnHealed;
	FLiveOpsOnRevive OnRevive;
	FOnCurrencyChanged OnCurrencyChanged;
	FLiveOpsOnItemUsed OnItemUsed;
	FLiveOpsOnOpenChest OnOpenChest;
	FLiveOpsOnMovement OnMovement;

	FConnectionStatusChanged OnConnectionStatusChanged;

#if !UE_BUILD_SHIPPING
	MinecraftAPIAuthData APIAuthDataCache = {"", "", "", {}};
#endif

private:
	template <typename CompletionCallback>
	void RequestUpdateInternalAsync(const MultiRequest::Sequence&, const CompletionCallback&) const;
	void OnSuccessfulMinecraftAPIAuthentication(const MinecraftAPIAuthData&, const XAuthData&);
	void OnFailedMinecraftAPIAuthentication(const XAuthData&);
	bool IsAuthenticated() const;
	MultiRequest::Step GetDefaultRefreshStep() const;

	void RegisterHeartbeat(const MultiRequest::Sequence&, float frequency);

	std::unique_ptr<LeaderboardsHandler> LeaderboardHandler;
	std::unique_ptr<ChallengesHandler> ChallengeHandler;
	std::unique_ptr<SeasonsHandler> SeasonHandler;
	std::unique_ptr<ProgressHandler> ProgressHandler;
	std::unique_ptr<AdventurePointsHandler> AdventurePointsHandler;
	std::unique_ptr<RewardsHandler> RewardsHandler;

	std::vector<std::unique_ptr<PollBase>> PollRepository;
	UGameInstance* GameInstance;
	FDelegateHandle TickTimerHandle;
	FTickerDelegate TickDelegate;
	FDelegateHandle OnClientSuccessfullyAuthenticatedHandler;
	FDelegateHandle OnClientAuthenticationFailedHandler;
};

}
}
