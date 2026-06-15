#include "ProgressHandler.h"
#include "IDungeonsAPIClient.h"
#include "ChallengesHandler.h"
#include "DungeonsGameInstance.h"
#include "util/SeasonsCommon.h"

namespace online
{
namespace liveops
{

minecraft::api::ProgressRequest MakeProgressRequest(const std::string& name, const int64& score) {
	auto time = minecraft::api::Datetime();
	return {
		make_shared<std::string>(name),
		make_shared<int64>(score),
		make_shared<std::string>(common::GenerateUUID(time.timeMillis.time_since_epoch().count()))
	};
}

ProgressHandler::ProgressHandler(std::shared_ptr<LiveOpsClient> apiClient, UGameInstance* GameInstance)
	: LiveIF(std::move(apiClient), GameInstance) {
}

void ProgressHandler::Init() {
	if (auto dungeonsGameInstance = Cast<UDungeonsGameInstance>(GameInstance)) {
		auto tickCallback = [&] () {
			EngageQueue(false);
		};
		dungeonsGameInstance->GetTimerManager().SetTimer(QueueTickTimer, tickCallback, 1, true);
	}
}

void ProgressHandler::Teardown() {
	EngageQueue(true);
	if (auto dungeonsGameInstance = Cast<UDungeonsGameInstance>(GameInstance)) {
		dungeonsGameInstance->GetTimerManager().ClearTimer(QueueTickTimer);
	}
}

void ProgressHandler::UpdateProgressions(const std::vector<minecraft::api::Progress>& progresses) {
	for (const auto& progress : progresses) {
		CurrentProgressions[progress.name] = { progress.name, progress.score };
	}
}

void ProgressHandler::Request() {
	MultiRequestPartial(std::shared_ptr<MultiRequest>());
}

void ProgressHandler::MultiRequestPartial(std::shared_ptr<MultiRequest> multiRequest) {
	LiveOpsClient::Callback<minecraft::api::GetProgressResponse> callback = [this, multiRequest](minecraft::api::HttpServiceResponse<minecraft::api::GetProgressResponse> response) {
		if (response.successful()) {
			if (auto body = response.getBody()) {
				UpdateProgressions(body->progresses);
			}
		} else {
			UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Failed to fetch progression: %d"), response.getStatus());
		}

		if (multiRequest) {
			multiRequest->CompleteSingleRequest(response);
		}
	};
	mApiClient->Request<DungeonsEndpoint::PROGRESS_GET>(callback);
}

LiveOpsClient::Callback<minecraft::api::PostProgressResponse> ProgressHandler::CreatePostProgressCallback(const minecraft::api::ProgressPost& request) {
	return [this, request = request] (minecraft::api::HttpServiceResponse<minecraft::api::PostProgressResponse> response) {
		if (!response.successful()) {
			UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Failed to write (%d) progression keys: %d"), request.progress->size(), response.getStatus());
			UE_LOG(LogLiveOps, Warning, TEXT("[LiveOps] Adding them back to queue to retry later."));

			for (const auto& p : *request.progress) {
				UnhandledProgressions.push_back(p);
			}
		}
	};
}

void ProgressHandler::AddToQueue(const minecraft::api::ProgressRequest& request) {
	UnhandledProgressions.push_back(request);
}

void ProgressHandler::EngageQueue(bool fireAndForget) {
	if (UnhandledProgressions.empty()) {
		return;
	}
	minecraft::api::ProgressPost output {
		std::make_shared<std::vector<minecraft::api::ProgressRequest>>(UnhandledProgressions.begin(), UnhandledProgressions.end())
	};
	UnhandledProgressions.clear();
	if (fireAndForget) {
		mApiClient->Request<DungeonsEndpoint::PROGRESS_POST, minecraft::api::PostProgressResponse>([](auto) {}, output);
	} else {
		mApiClient->Request<DungeonsEndpoint::PROGRESS_POST>(CreatePostProgressCallback(output), output);
	}
}

TOptional<int64> ProgressHandler::GetStatValue(const std::string& name) {
	auto it = CurrentProgressions.find(name);
	if (it == CurrentProgressions.end()) {
		return {};
	}
	return it->second.value;
}

const ProgressHandler::List& ProgressHandler::GetAllStats() const {
	return CurrentProgressions;
}

#if !UE_BUILD_SHIPPING
void ProgressHandler::ResetAllProgression(const LiveOpsClient::Callback<minecraft::api::ResetAllProgressResponse>& callback) {
	CurrentProgressions.clear();
	UnhandledProgressions.clear();
	mApiClient->Request<DungeonsEndpoint::PROGRESS_RESET>(callback);
}
#endif

}
}
