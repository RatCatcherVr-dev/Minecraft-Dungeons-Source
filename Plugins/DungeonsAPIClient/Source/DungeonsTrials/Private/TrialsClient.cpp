#include "CoreMinimal.h"
#include "TrialsClient.h"
#include "IDungeonsAuth.h"
#include "modules/trials/TrialsServiceClient.h"
#include "IDungeonsClient.h"

DEFINE_LOG_CATEGORY(LogDungeonsTrials)

void TrialsClient::Request() {
	if (const auto client = GetClient()) {
		const auto onResponse = [=](minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse> response) {
			OnTrialsLoaded.Broadcast(response);
		};
		client->getServiceClient<minecraft::api::TrialsServiceClient>()->getTrialsByOffset("dungeons", "0000", onResponse);
	} else {
		UE_LOG(LogDungeonsTrials, Warning, TEXT("Unable to send trials request. No authenticated client available."));
		OnTrialsLoadFailed.Broadcast();
	}
}

shared_ptr<minecraft::api::MinecraftClient> TrialsClient::GetClient() {
	if (IDungeonsAuth::IsAvailable()) {
		return IDungeonsAuth::Get().Auth()->GetClient();
	}
	return nullptr;
}
