#include "AnonymousTrialsClient.h"
#include "modules/trials/TrialsServiceClient.h"
#include "TimezoneUtil.h"
#include "ClientStringUtil.h"
#include "IDungeonsClient.h"

shared_ptr<minecraft::api::MinecraftClient> AnonymousTrialsClient::GetClient() {
	if (IDungeonsClient::IsAvailable()) {
		return IDungeonsClient::Get().CreateMinecraftClient();
	}
	return nullptr;
}

void AnonymousTrialsClient::Request() {
	if (const auto client = GetClient()) {
		const auto onResponse = [=](minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse> response) {
			OnTrialsLoaded.Broadcast(response);
		};

		client->getServiceClient<minecraft::api::TrialsServiceClient>()->getTrialsByOffsetAnonymous(
			"dungeons", "0000", onResponse);
	}
	else {
		UE_LOG(LogDungeonsTrials, Warning, TEXT("Unable to send trials request. No authenticated client available."));
		OnTrialsLoadFailed.Broadcast();
	}
}
