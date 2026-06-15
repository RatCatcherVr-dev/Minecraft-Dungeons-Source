#include "Dungeons.h"
#include "TrialsProvider.h"
#include "IDungeonsAPIClient.h"
#include "game/mission/trial/TrialsParser.h"

namespace online {
	namespace minecraftapi {
		bool isValid(minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse>& response) {
			if (response.successful()) {
				if (response.getBody()) {
					return true;
				}
					
				UE_LOG(LogTrials, Warning, TEXT("Backend response for trials was empty."))
			} else {
				UE_LOG(LogTrials, Warning, TEXT("Backend response for trials failed with status %d."), response.getStatus());
			}	

			return false;
		}
	}
}

void UTrialsProvider::LoadTrials() {	
	if (const auto trials = GetTrialsClient()) {
		if (!trials->OnTrialsLoaded.IsBoundToObject(this)) {
			OnTrialsLoadedHandle = trials->OnTrialsLoaded.AddUObject(this, &UTrialsProvider::OnTrialsLoaded);
			OnTrialsLoadFailedHandle = trials->OnTrialsLoadFailed.AddUObject(this, &UTrialsProvider::OnTrialsLoadFailed);
			trials->Request();
		}
	} else {
		UE_LOG(LogTrials, Warning, TEXT("Unable to load trials. DungeonsAPIClient module is unavailable."));
	}
}

FReplicatableTrialDefPack UTrialsProvider::GetCachedTrials() const {
	return CachedTrialDefs;
}

std::shared_ptr<TrialsClient> UTrialsProvider::GetTrialsClient() const {
	if (IDungeonsAPIClient::IsAvailable()) {
		return IDungeonsAPIClient::Get().Trials()->Authenticated();
	}		
	return nullptr;
}

void UTrialsProvider::OnTrialsLoaded(minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse>& response) {
	if (online::minecraftapi::isValid(response)) {
		const auto parsedResponse = trial::parseDailyTrials(response);
		CachedTrialDefs.Set(parsedResponse);
		if (parsedResponse.Num() > 0) {
			OnDailyTrialsProvided.Broadcast(parsedResponse);
		} else {
			UE_LOG(LogTrials, Warning, TEXT("Parsed backend response contained no daily trials."))
		}
	}

	UnregisterListeners();
}

void UTrialsProvider::OnTrialsLoadFailed() {
	UE_LOG(LogTrials, Warning, TEXT("Failed to load daily trials."))
	UnregisterListeners();
}

void UTrialsProvider::UnregisterListeners() const {
	if (const auto trials = GetTrialsClient()) {
		trials->OnTrialsLoaded.Remove(OnTrialsLoadedHandle);
	}
}
