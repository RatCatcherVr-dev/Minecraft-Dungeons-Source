#pragma once

#include "AnalyticsEvent.h"
#include "PlayfabPlayer.h"
#include "PlayfabServices.h"
#include <mutex>
#include <chrono>
#include <JsonValue.h>
#include <SharedPointer.h>
#include <Array.h>
#include <Runtime/Online/HTTP/Public/Http.h>
#include <DataModels.h>

enum class SendAnalyticEventResult
{
	UnableToAuthenticate,
	AuthenticationInProcess,
	AuthenticationNeeded,
	Success
};

class PlayFabClient
{
public:
	PlayFabClient(FString titleID) : mTitleId(titleID), mLastPayloadSend(std::chrono::steady_clock::now()){}

	SendAnalyticEventResult SendAnalyticsEvent(PlayFabPlayer* player, AnalyticsEvent& event);

	void Tick();
	void OnShutdown();

	bool AuthenticatePlayer(PlayFabPlayer*, AuthenticationConfig);
	bool LinkAccounts(PlayFabPlayer*, AccountLinkConfig);
	bool UnlinkPsnAccountFromXBL(PlayFabPlayer*, AccountUnlinkConfig);

	bool GetLinkStatus(PlayFabPlayer*, TFunction<void(bool success)> callback);
	bool SetGamertagHint(PlayFabPlayer*, FString xblGamerTag);
	bool GetGamertagHint(PlayFabPlayer*, TFunction<void(FString gamerTag)> callback);

	FString GetTitleId() const;
	void PlayfabCleanup(PlayFabPlayer*);
private:
	SendAnalyticEventResult GetAuthenticationStatus(PlayFabPlayer*);
	SendAnalyticEventResult AuthenticateIfNeeded(PlayFabPlayer*);

	void SendEventPayloads();

	FString mTitleId{};
	std::chrono::steady_clock::time_point mLastPayloadSend;
	std::mutex mAuthPlayerMutex;
	std::mutex mEventPayloadsMutex;
	TMap<FString, int> mPlayersAuthFailed;
	TArray<TSharedRef<FJsonValueObject>> mEventPayloads;
	FString mPlayfabEntityToken;
	bool mTokenExpiredResponse{ false };

	enum class AccountLinkingResult {
		Success = 0,
		PsnAlreadyLinked = -1,
		MsaAlreadyLinked = 1011,
	};
};
