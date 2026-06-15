#include "LiveOps.h"
#include "IDungeonsAPIClient.h"
#include "MinecraftAPIAuthData.h"
#include "XAuthData.h"

#include <memory>
#include <vector>
DEFINE_LOG_CATEGORY(LogLiveOps)

namespace online
{
namespace liveops
{

LiveOps::LiveOps(UGameInstance* InGameInstance) : GameInstance(InGameInstance) {
	ChallengeHandler = std::make_unique<ChallengesHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);
	LeaderboardHandler = std::make_unique<LeaderboardsHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);
	SeasonHandler = std::make_unique<SeasonsHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);
	ProgressHandler = std::make_unique<online::liveops::ProgressHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);
	AdventurePointsHandler = std::make_unique<online::liveops::AdventurePointsHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);
	RewardsHandler = std::make_unique<online::liveops::RewardsHandler>(IDungeonsAPIClient::Get().LiveOps(), GameInstance);

	ChallengeHandler->Init();
	LeaderboardHandler->Init();
	SeasonHandler->Init();
	ProgressHandler->Init();
	AdventurePointsHandler->Init();
	RewardsHandler->Init();

	OnClientSuccessfullyAuthenticatedHandler = IDungeonsAPIClient::Get().Auth()->OnClientSuccessfullyAuthenticated.AddRaw(this, &LiveOps::OnSuccessfulMinecraftAPIAuthentication);
	OnClientAuthenticationFailedHandler = IDungeonsAPIClient::Get().Auth()->OnClientAuthenticationFailed.AddRaw(this, &LiveOps::OnFailedMinecraftAPIAuthentication);
	IDungeonsAPIClient::Get().LiveOps()->OnConnectionStatusChanged.AddLambda([&](EMinecraftAPIConnectionStatus status) {
		OnConnectionStatusChanged.Broadcast(status);
		switch (status) {
			case EMinecraftAPIConnectionStatus::Connected:
				UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Connection status changed: Connected"));
				break;
			case EMinecraftAPIConnectionStatus::TimingOut:
				UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Connection status changed: TimingOut"));
				break;
			case EMinecraftAPIConnectionStatus::GameClientTooOld:
				UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Connection status changed: GameClientTooOld"));
				break;
			case EMinecraftAPIConnectionStatus::NoConnection:
			default:
				UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Connection status changed: No connection!!"));
				break;
		}
	});

	RegisterHeartbeat({GetDefaultRefreshStep()}, 30.0f);
}

LiveOps::~LiveOps() {
	IDungeonsAPIClient::Get().LiveOps()->CancelAllRetries();
	IDungeonsAPIClient::Get().Auth()->OnClientSuccessfullyAuthenticated.Remove(OnClientSuccessfullyAuthenticatedHandler);
	IDungeonsAPIClient::Get().Auth()->OnClientAuthenticationFailed.Remove(OnClientAuthenticationFailedHandler);
	ChallengeHandler->Teardown();
	LeaderboardHandler->Teardown();
	SeasonHandler->Teardown();
	ProgressHandler->Teardown();
	AdventurePointsHandler->Teardown();
	RewardsHandler->Teardown();
}

void LiveOps::RequestDataUpdateAsync() const {
	auto onCompletion = [this](UpdateRequestStatus status) {
		this->OnDataUpdate.Broadcast(status);
	};

	RequestUpdateInternalAsync({
		{ AdventurePointsHandler.get(), ChallengeHandler.get(), ProgressHandler.get(), SeasonHandler.get(), RewardsHandler.get() }, 
		GetDefaultRefreshStep()
	}, onCompletion);
}

void LiveOps::RequestSeasonViewUpdateAsync() const {
	auto onCompletion = [this](UpdateRequestStatus status) {
		SeasonView view = GetSeasonView();
		OnSeasonViewUpdate.Broadcast(status, view);
	};

	RequestUpdateInternalAsync({ 
		{ SeasonHandler.get(), RewardsHandler.get(), ProgressHandler.get() },
		GetDefaultRefreshStep()
	}, onCompletion);
}

void LiveOps::RequestChallengesViewUpdateAsync() const {
	auto onCompletion = [this](UpdateRequestStatus status) {
		ChallengesView view = GetChallengesView();
		OnChallengesViewUpdate.Broadcast(status, view);
	};

	RequestUpdateInternalAsync({
		{ ChallengeHandler.get(), RewardsHandler.get(), ProgressHandler.get() },
		GetDefaultRefreshStep()
	}, onCompletion);
}

void LiveOps::RequestEventViewUpdateAsync() const {
	auto onCompletion = [this](UpdateRequestStatus status) {
		TOptional<EventView> view = GetEventView();
		OnEventViewUpdate.Broadcast(status, view);
	};

	RequestUpdateInternalAsync({
		{ SeasonHandler.get(), RewardsHandler.get(), ProgressHandler.get(), ChallengeHandler.get() },
		GetDefaultRefreshStep()
	}, onCompletion);
}

SeasonView LiveOps::GetSeasonView() const {
	return SeasonViewBuilder::Build(SeasonHandler.get(), RewardsHandler.get(), ProgressHandler.get());
}

ChallengesView LiveOps::GetChallengesView() const {
	return ChallengesViewBuilder::Build(ChallengeHandler.get(), RewardsHandler.get(), ProgressHandler.get());
}

TOptional<EventView> LiveOps::GetEventView() const {
	return EventViewBuilder::Build(SeasonHandler.get(), RewardsHandler.get(), ProgressHandler.get(), ChallengeHandler.get());
}

template <typename CompletionCallback>
void LiveOps::RequestUpdateInternalAsync(const MultiRequest::Sequence& steps, const CompletionCallback& onCompletion) const {
	auto onCompletionWithChecks = [this, onCompletion](const MultiRequest* request) {
		if (request->WasSuccessful()) {
			onCompletion(UpdateRequestStatus::Success);
		} else {
			UE_LOG(LogLiveOps, Warning, TEXT("RequestUpdateInternalAsync failed (MultiRequest failed)"));
			onCompletion(UpdateRequestStatus::Failure);
		}
	};

	if (IsAuthenticated()) {
		MultiRequest::Initiate(steps, onCompletionWithChecks);
	} else {
		UE_LOG(LogLiveOps, Warning, TEXT("RequestUpdateInternalAsync failed (not authenticated)"));
		onCompletion(UpdateRequestStatus::Failure);
	}
}

EMinecraftAPIConnectionStatus LiveOps::GetConnectionStatus() const {
	if (auto liveOps = IDungeonsAPIClient::Get().LiveOps()) {
		return liveOps->GetConnectionStatus();
	}
	return EMinecraftAPIConnectionStatus::NoConnection;
}

void LiveOps::OnSuccessfulMinecraftAPIAuthentication(const MinecraftAPIAuthData& APIAuthData, const XAuthData& AuthData) {
#if !UE_BUILD_SHIPPING
	APIAuthDataCache = APIAuthData;
#endif

	IDungeonsAPIClient::Get().Auth()->OnClientSuccessfullyAuthenticated.Remove(OnClientSuccessfullyAuthenticatedHandler);
	IDungeonsAPIClient::Get().Auth()->OnClientAuthenticationFailed.Remove(OnClientAuthenticationFailedHandler);
	RequestDataUpdateAsync();
}

void LiveOps::OnFailedMinecraftAPIAuthentication(const XAuthData& AuthData) {
}

bool LiveOps::IsAuthenticated() const {
	return IDungeonsAPIClient::Get().Auth() && IDungeonsAPIClient::Get().Auth()->GetClient();
}

MultiRequest::Step LiveOps::GetDefaultRefreshStep() const {
	return { RewardsHandler.get(), ProgressHandler.get() };
}

void LiveOps::RegisterHeartbeat(const MultiRequest::Sequence& sequence, float rate) {
	Heartbeat::MultiRequest multiRequest = [this, sequence](std::function<void(UpdateRequestStatus)> onCompletion) {
		RequestUpdateInternalAsync(sequence, onCompletion);
	};
	auto heartBeat = std::make_unique<Heartbeat>(GameInstance, multiRequest, rate);
	heartBeat->Start();
	PollRepository.push_back(std::move(heartBeat));
}

LeaderboardsHandler* LiveOps::GetLeaderboards() const {
	return LeaderboardHandler.get();
}

ChallengesHandler* LiveOps::GetChallenges() const {
	return ChallengeHandler.get();
}

SeasonsHandler* LiveOps::GetSeasonHandler() const {
	return SeasonHandler.get();
}

ProgressHandler* LiveOps::GetProgressHandler() const {
	return ProgressHandler.get();
}

AdventurePointsHandler* LiveOps::GetAdventurePointsHandler() const {
	return AdventurePointsHandler.get();
}

RewardsHandler* LiveOps::GetRewardsHandler() const {
	return RewardsHandler.get();
}

}
}
