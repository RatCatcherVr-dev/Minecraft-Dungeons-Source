#pragma once

#include "CoreMinimal.h"
#include <memory>

#include "core/MinecraftClient.h"
#include "modules/LiveOps/LiveOpsEndpoints.h"
#include "Engine/Classes/Engine/EngineTypes.h"
#include "Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "modules/LiveOps/LiveOpsServiceClient.h"
#include "ConnectionStatus.h"

namespace minecraft {
	namespace api {
		struct ProgressPost;
	}
}

namespace liveops {
	struct Stat;
}

class LiveOpsClient;

DECLARE_MULTICAST_DELEGATE_OneParam(FConnectionStatusChanged, EMinecraftAPIConnectionStatus);

class DUNGEONSLIVEOPS_API LiveOpsClient {
public:
	LiveOpsClient() = default;
	LiveOpsClient(const LiveOpsClient&) = delete;
	LiveOpsClient& operator=(const LiveOpsClient&) = delete;
	void CancelAllRetries();

	FConnectionStatusChanged OnConnectionStatusChanged;
	const EMinecraftAPIConnectionStatus& GetConnectionStatus() const { return CurrentConnectionStatus; }

	template <typename Response>
	using Callback = std::function<void(minecraft::api::HttpServiceResponse<Response>)>;

	template <typename Response>
	using RequestCallback = std::function<void(const Callback<Response>&)>;

	template <minecraft::api::endpoint::Dungeons Endpoint, typename Response, typename Post = std::string>
	void Request(Callback<Response> callback = [](Response) {}, Post post = Post());

private:
	static constexpr int Retries = 10;
	static constexpr int RetryThresholdNoConnection = 1;

	template <minecraft::api::endpoint::Dungeons Endpoint, typename Response, typename Post>
	void DoRequest(const Callback<Response>& callback, Post post);
	std::shared_ptr<minecraft::api::MinecraftClient> GetAuthenticatedClient() const;
	std::shared_ptr<minecraft::api::LiveOpsServiceClient> GetServiceClient() const;
	std::function<TOptional<int>(int)> getTimeoutFunction(int nrRetries);
	void UpdateConnectionStatus(EMinecraftAPIConnectionStatus newStatus);
	bool GameClientSupportsLiveOpsServiceApi() const;

	std::vector<std::unique_ptr<FTimerHandle>> TimerHandles;

	FTimerHandle* GetTimerHandle();
	std::function<void(FTimerHandle*)> GetRemoveTimerHandleFunction();
	template <typename Response>
	std::function<void(int, minecraft::api::HttpServiceResponse<Response>)> GetConnectionStatusFunction();

	EMinecraftAPIConnectionStatus CurrentConnectionStatus = EMinecraftAPIConnectionStatus::NoConnection;
};

namespace internal
{

template <typename Response>
std::function<bool(minecraft::api::HttpServiceResponse<Response>)> getRetryPredicate() {
	return [] (minecraft::api::HttpServiceResponse<Response> response) {
		if (response.successful()) {
			return false;
		}
		if (response.getStatus() == EHttpResponseCodes::RequestTimeout) {
			return true;
		}
		if (response.getStatus() == EHttpResponseCodes::TooManyRequests) {
			return true;
		}
		if (response.getStatus() >= EHttpResponseCodes::BadRequest &&
			response.getStatus() < EHttpResponseCodes::ServerError) {
			return false;
		}
		if (response.getStatus() >= EHttpResponseCodes::ServerError) {
			return true;
		}
		return false;
	};
}


template <typename Response>
struct RetryConfig {
	const LiveOpsClient::RequestCallback<Response> request;
	const LiveOpsClient::Callback<Response> callback;
	FTimerManager* timerManager;
	std::function<TOptional<int>(int)> getTimeout;
	std::function<void(FTimerHandle*)> removeTimerHandle;
	std::function<bool(minecraft::api::HttpServiceResponse<Response>)> retryPredicate;
	FTimerHandle* timerHandle;
	std::function<void(int, minecraft::api::HttpServiceResponse<Response>)> updateConnectionStatus;
};

template <typename Response>
LiveOpsClient::Callback<Response> retryFunction(RetryConfig<Response> config, int retries = 0) {
	return [config, retries] (const minecraft::api::HttpServiceResponse<Response>& response) mutable {
		auto timeout = config.getTimeout(retries);
		config.updateConnectionStatus(retries, response);
		if (timeout.IsSet() && config.retryPredicate(response)) {
			config.timerHandle->Invalidate();
			auto timeoutRetry = [request = config.request, retry = retryFunction<Response>(config, retries + 1)] () {
				request(retry);
			};

			config.timerManager->SetTimer(*config.timerHandle, timeoutRetry, timeout.GetValue(), false);
		} else {
			config.removeTimerHandle(config.timerHandle);
			config.callback(response);
		}
	};
}
	
}

template <typename Response>
std::function<void(int, minecraft::api::HttpServiceResponse<Response>)> LiveOpsClient::GetConnectionStatusFunction() {
	return [&](int retryCount, minecraft::api::HttpServiceResponse<Response> response) {
		switch (CurrentConnectionStatus) {
		case EMinecraftAPIConnectionStatus::Connected:
			if (!response.successful())
				UpdateConnectionStatus(EMinecraftAPIConnectionStatus::TimingOut);
			break;
		case EMinecraftAPIConnectionStatus::TimingOut:
			if (response.successful())
				UpdateConnectionStatus(EMinecraftAPIConnectionStatus::Connected);
			else if (retryCount >= RetryThresholdNoConnection)
				UpdateConnectionStatus(EMinecraftAPIConnectionStatus::NoConnection);
			break;
		case EMinecraftAPIConnectionStatus::NoConnection:
			if (response.successful())
				UpdateConnectionStatus(EMinecraftAPIConnectionStatus::Connected);
			break;
		case EMinecraftAPIConnectionStatus::GameClientTooOld:
			// Do nothing, this state can only be escaped by updating the game client.
			break;
		default:
			checkNoEntry();
			break;
		}
	};
}

template <minecraft::api::endpoint::Dungeons Endpoint, typename Response, typename Post>
void LiveOpsClient::DoRequest(const Callback<Response>& callback, Post post) {
	if (const auto client = GetAuthenticatedClient()) {
		if (const auto serviceClient = GetServiceClient()) {
			auto request = [serviceClient, post](const Callback<Response>& callback) {
				serviceClient->request<Endpoint>(callback, post);
			};

			auto retryingCallback = internal::retryFunction<Response>({
				request,
				callback,
				client->getTimerManager(),
				getTimeoutFunction(Retries),
				GetRemoveTimerHandleFunction(),
				internal::getRetryPredicate<Response>(),
				GetTimerHandle(),
				GetConnectionStatusFunction<Response>()
				});

			request(retryingCallback);
		}
	}
}

template <minecraft::api::endpoint::Dungeons Endpoint, typename Response, typename Post>
void LiveOpsClient::Request(Callback<Response> callback, Post post) {
	if (GetAuthenticatedClient()) {
		if (GameClientSupportsLiveOpsServiceApi()) {
			DoRequest<Endpoint, Response, Post>(callback, post);
		} else {
			UpdateConnectionStatus(EMinecraftAPIConnectionStatus::GameClientTooOld);
			callback(minecraft::api::HttpServiceResponse<Response>{EHttpResponseCodes::ServerError});
		}
	} else {
		UpdateConnectionStatus(EMinecraftAPIConnectionStatus::NoConnection);
		callback(minecraft::api::HttpServiceResponse<Response>(EHttpResponseCodes::ServerError));
	}
}
