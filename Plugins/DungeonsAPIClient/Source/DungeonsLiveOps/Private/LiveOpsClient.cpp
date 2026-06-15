#include "LiveOpsClient.h"
#include "IDungeonsAuth.h"


std::function<TOptional<int>(int)> LiveOpsClient::getTimeoutFunction(int nrRetries) {
	return [nrRetries](int retry) -> TOptional<int> {
		if (retry >= nrRetries) {
			return {};
		}
		return { std::min(5 * static_cast<int>(pow(2, retry)), 60) };
	};
}

void LiveOpsClient::UpdateConnectionStatus(EMinecraftAPIConnectionStatus newStatus) {
	const EMinecraftAPIConnectionStatus previousStatus = CurrentConnectionStatus;
	CurrentConnectionStatus = newStatus;

	if (CurrentConnectionStatus != previousStatus) {
		OnConnectionStatusChanged.Broadcast(CurrentConnectionStatus);
	}
}

bool LiveOpsClient::GameClientSupportsLiveOpsServiceApi() const {
	auto toVersion = [](const std::string& version) -> std::vector<int> {
		std::vector<int> result;
		std::istringstream parser{ version };

		result.reserve(2);

		while (!parser.eof()) {
			int component = 0;
			parser >> component;
			parser.get();
			result.push_back(component);
		}

		return result;
	};

	auto equalOrGreaterThan = [](const std::vector<int>& lhs, const std::vector<int>& rhs) {
		return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	};

#if UE_BUILD_SHIPPING
	if (const auto client = GetAuthenticatedClient()) {
		return equalOrGreaterThan(toVersion(client->getGameVersion()), toVersion(client->getRequiredLiveOpsGameVersion()));
	}
	else {
		return false;
	}
#else
	return true;
#endif
}

FTimerHandle* LiveOpsClient::GetTimerHandle() {
	TimerHandles.push_back(std::make_unique<FTimerHandle>());
	return TimerHandles.back().get();
}

std::function<void(FTimerHandle*)> LiveOpsClient::GetRemoveTimerHandleFunction() {
	return [&] (FTimerHandle* timerHandle) {
		auto it = std::find_if(TimerHandles.begin(), TimerHandles.end(), [&] (const std::unique_ptr<FTimerHandle>& handle) {
			return *timerHandle == *handle.get();
		});
		if (it != TimerHandles.end()) {
			TimerHandles.erase(it);
		}
	};
}

void LiveOpsClient::CancelAllRetries() {
	if (const auto client = GetAuthenticatedClient()) {
		for (const auto& timerHandle : TimerHandles) {
			if (timerHandle->IsValid()) {
				client->getTimerManager()->ClearTimer(*timerHandle);
			}
		}
	}
	TimerHandles.clear();
}


std::shared_ptr<minecraft::api::MinecraftClient> LiveOpsClient::GetAuthenticatedClient() const {
	return IDungeonsAuth::Get().Auth() ? IDungeonsAuth::Get().Auth()->GetClient() : std::shared_ptr<minecraft::api::MinecraftClient>(nullptr);
}

std::shared_ptr<minecraft::api::LiveOpsServiceClient> LiveOpsClient::GetServiceClient() const {
	if (const auto client = GetAuthenticatedClient()) {
		return client->getServiceClient<minecraft::api::LiveOpsServiceClient>();
	}
	return {};
}
