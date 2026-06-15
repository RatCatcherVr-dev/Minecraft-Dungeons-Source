// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlayfabServices.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "PlayfabServicesSettings.h"
#include "AnalyticsService.h"

#include "Runtime/Core/Public/Containers/Ticker.h"
#include "Async/AsyncWork.h"
#include "CoreDelegates.h"
#include "async.h"
#include "ConfigCacheIni.h"
#include "OnlineUserCloudCommon.h"

PlayFabPlayer::AuthTokenFunction PlayfabServices::AuthoTokenFunc = nullptr;
TQueue<PlayfabServicesEvent, EQueueMode::Mpsc> PlayfabServices::queuedAnalyticsEvents{};

constexpr auto MAX_CACHED_EVENTS = 500;

DEFINE_LOG_CATEGORY(LogPlayfabServices);

static TUniquePtr<AnalyticsService> gAnalyticService;
static PlayfabServicesInitState kInitState = PlayfabServicesInitState::NotInitialized;
static FDelegateHandle gTickTimerHandle;

class FAnalyticServiceTickAsyncTask : public FNonAbandonableTask {
public:
	friend class FAsyncTask<FAnalyticServiceTickAsyncTask>;

	FAnalyticServiceTickAsyncTask(PlayfabServices::DequeueFunction deque)
		: Dequeue(deque) {
	}

	void DoWork() {
		if (gAnalyticService) {
			Dequeue();
			gAnalyticService->tick();
		}
	}

	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAnalyticServiceTickAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
private:
	PlayfabServices::DequeueFunction Dequeue;
};

TUniquePtr< FAsyncTask< FAnalyticServiceTickAsyncTask > > g_AnalyticsTickTask = nullptr;


void PlayfabServices::AnalyticsTick() {
	if (!queuedAnalyticsEvents.IsEmpty()) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_FAnalyticServiceTickAsyncTask_Launch);
		if (g_AnalyticsTickTask && g_AnalyticsTickTask->IsDone()) {
			g_AnalyticsTickTask->StartBackgroundTask();
		}
	}
}

void PlayfabServices::AddPlayerToCache(const FString& PlayerId) {
	PlayFabPlayer p{ PlayerId };
	p.SetAuthenticationMethod(AuthoTokenFunc);
	gAnalyticService->AddPlayerToCache(std::move(p));
}

namespace internal
{
	bool isDevelopmentMode() {
		auto developmentMode = false;

		if (GConfig) {
			GConfig->GetBool(TEXT("Environment"), TEXT("Development"), developmentMode, GEngineIni);
		}

		return developmentMode;
	}
}

void PlayfabServices::StartupModule() {
    UE_LOG(LogPlayfabServices, Log, TEXT("Starting Mojang Services module"));

    const UPlayfabServicesSettings* settings = GetDefault<UPlayfabServicesSettings>();
	if (!settings) {
        UE_LOG(LogPlayfabServices, Error, TEXT("No PlayfabServices settings found."));
        kInitState = PlayfabServicesInitState::ConnectionFailure;
        return;
	}

	FString titleId;
	if (internal::isDevelopmentMode()) {
		titleId = settings->DevelopmentTitleId;
	} else {
		titleId = settings->TitleId;
	}

    if (titleId.IsEmpty()) {
        UE_LOG(LogPlayfabServices, Error, TEXT("Title Id needs to be specified in configuration."));
        kInitState = PlayfabServicesInitState::ConnectionFailure;
        return;
    }
    
    FString cacheFile = settings->PropertyCacheFile;
    gAnalyticService = MakeUnique<AnalyticsService>(TCHAR_TO_UTF8(*cacheFile), TCHAR_TO_UTF8(*titleId));

	g_AnalyticsTickTask = MakeUnique< FAsyncTask< FAnalyticServiceTickAsyncTask > >([&]() { PlayfabServices::DequeueTelemetry(); });

    gTickTimerHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float time) -> bool {
		AnalyticsTick();
       return true;
    }),.1f);

    kInitState = PlayfabServicesInitState::Success;
}


void PlayfabServices::ShutdownModule() {
	UE_LOG(LogPlayfabServices, Log, TEXT("Shutting down Mojang Services module"));
	if (g_AnalyticsTickTask) {
		g_AnalyticsTickTask->EnsureCompletion();
		g_AnalyticsTickTask.Reset();
	}
	if (gAnalyticService) {
		gAnalyticService->shutDownService();
		gAnalyticService.Reset();
	}
	FTicker::GetCoreTicker().RemoveTicker(gTickTimerHandle);
}

PlayfabServicesInitState PlayfabServices::GetInitState() {
    return kInitState;
}

void PlayfabServices::SendTelemetry(const PlayfabServicesEvent& event) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry);

	if (kInitState == PlayfabServicesInitState::ConnectionFailure || 
		kInitState == PlayfabServicesInitState::NotInitialized ||
		!gAnalyticService.IsValid()) {
		return;
	}

	queuedAnalyticsEvents.Enqueue(event);
}

void PlayfabServices::DequeueTelemetry() {
	PlayfabServicesEvent DequeuedEvent;

	QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry_AuthoToken);
	if (!AuthoTokenFunc) {
		return;
	}

	while (queuedAnalyticsEvents.Dequeue(DequeuedEvent)) {
		AnalyticsEvent aEvent(DequeuedEvent.getUserId(), DequeuedEvent.getName());
		
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry_aEvent);
			aEvent.setShouldAggregate(DequeuedEvent.getShouldAggregate());
			aEvent.setAggregationTime(DequeuedEvent.getAggregationTime());
			aEvent.setProcessedRealtime(DequeuedEvent.getProcessedRealtime());
		}

		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry_getProperties);
			for (auto& prop : DequeuedEvent.getProperties()) {
				const auto& value = prop.second.getValue();
				auto propName = prop.first;
				aEvent.addProperty(propName, value);
			}
		}
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry_getMeasurements);

			for (auto& prop : DequeuedEvent.getMeasurements()) {
				const auto& value = prop.second.getValue();
				auto propName = prop.first;
				AnalyticsMeasurement::AggregationType aggType;
				switch (prop.second.getAggregationType()) {
				case PlayfabServicesMeasurement::AggregationType::Increment:
					aggType = AnalyticsMeasurement::AggregationType::Increment;
					break;
				case PlayfabServicesMeasurement::AggregationType::Sum:
					aggType = AnalyticsMeasurement::AggregationType::Sum;
					break;
				case PlayfabServicesMeasurement::AggregationType::Average:
					aggType = AnalyticsMeasurement::AggregationType::Average;
					break;
				case PlayfabServicesMeasurement::AggregationType::Max:
					aggType = AnalyticsMeasurement::AggregationType::Max;
					break;
				case PlayfabServicesMeasurement::AggregationType::Min:
					aggType = AnalyticsMeasurement::AggregationType::Min;
					break;
				default:
					continue;
				}

				aEvent.addMeasurement(propName, aggType, value);
			}
		}

		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_FPlayfabServices_SendTelemetry_recordTelemetry);
			gAnalyticService->recordTelemetry(aEvent);
		}

	}
}

PlayFabClient* PlayfabServices::GetPlayFabClient() {
	return gAnalyticService.IsValid() ? gAnalyticService->GetPlayFabClient() : nullptr;
}

PlayFabPlayer* PlayfabServices::GetPlayfabPlayer() {
	return gAnalyticService.IsValid() ? gAnalyticService->GetFirstPlayFabPlayer() : nullptr;
}

FString PlayfabServices::GetTitleId() {
	if (auto client = GetPlayFabClient()) {
		return client->GetTitleId();
	}
	return "";
}

FString PlayfabServices::GetLinkedXblAccount() {
	auto player = GetPlayfabPlayer();
	return player ? player->GetLinkedXblAccount() : "";
}

AccountLinkStatus PlayfabServices::GetAccountLinkStatus(FString xblAccount) {
	auto player = GetPlayfabPlayer();
	if (!player) {
		return AccountLinkStatus::Error;
	}
	auto linkedXblAccount = GetLinkedXblAccount();
	if (linkedXblAccount.IsEmpty())
		return AccountLinkStatus::NotLinked;
	return linkedXblAccount != xblAccount ? AccountLinkStatus::ThisPsnAlreadyLinkedToDifferentMSA : AccountLinkStatus::Linked;
}

bool PlayfabServices::LinkAccounts(AccountLinkConfig config) {
	auto client = GetPlayFabClient();
	if (!client) {
		return false;
	}
	auto player = GetPlayfabPlayer();
	if (!player) {
		return false;
	}
	return client->LinkAccounts(player, config);
}

bool PlayfabServices::UnlinkXBLAccount(AccountUnlinkConfig config) {
	if (config.psnAuthToken.IsEmpty())
		return false;
	auto client = GetPlayFabClient();
	auto player = GetPlayfabPlayer();
	if (!player || !client)
		return false;
	config.xblAccount = player->GetLinkedXblAccount();
	if (config.xblAccount.IsEmpty())
		return false;
	return client->UnlinkPsnAccountFromXBL(player, config);
}

bool PlayfabServices::SetGamertagHint(FString xblGamerTag) {
	auto client = GetPlayFabClient();
	auto player = GetPlayfabPlayer();
	if (!player || !client)
		return false;
	if (GetLinkedXblAccount().IsEmpty())
		return false;
	return client->SetGamertagHint(player, xblGamerTag);
}

bool PlayfabServices::GetGamertagHint(TFunction<void(FString gamerTag)> callback) {
	auto client = GetPlayFabClient();
	auto player = GetPlayfabPlayer();
	if (!player || !client)
		return false;
	return client->GetGamertagHint(player, callback);
}

bool PlayfabServices::RefreshAuthenticationCredentials(const AuthenticationConfig& config) {
	auto player = GetPlayfabPlayer();
	if (!player) {
		return false;
	}
	player->ClearCredentials();
	return RegisterOnAuthenticationCallback(config);
}

bool PlayfabServices::RegisterOnAuthenticationCallback(const AuthenticationConfig& config) {
	auto player = GetPlayfabPlayer();
	if (!player)
		return false;
	if (player->HasCredentials()) {
		config.callback(true);
		return true;
	}
	player->AddAuthenticateCallback(config.callback);
	if (player->AuthenticationInProgress) {
		return true;
	}
	player->AuthenticationInProgress = true;
	return Authenticate(config);
}

bool PlayfabServices::Authenticate(const AuthenticationConfig& config) {
	auto client = GetPlayFabClient();
	auto player = GetPlayfabPlayer();
	if (!player || !client)
		return false;
	return client->AuthenticatePlayer(player, config);
}

#if CLOUDSAVE_PLASTERFIX_453486
void PlayfabServices::PlayfabCleanup() {
	auto client = GetPlayFabClient();
	auto player = GetPlayfabPlayer();
	if (player && client) {
		if (!PlayfabServices::queuedAnalyticsEvents.IsEmpty())
			AnalyticsTick();
		if (g_AnalyticsTickTask) {
			while (!g_AnalyticsTickTask->IsDone())
				FPlatformProcess::Sleep(0.1f);
		}
		gAnalyticService->shutDownService();
		client->PlayfabCleanup(player);
		gAnalyticService->RemovePlayerFromCache(*player);
	}
}
#endif

IMPLEMENT_MODULE(PlayfabServices, PlayfabServices)
