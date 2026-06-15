#pragma once

#include "CoreMinimal.h"
#include "modules/trials/TrialsApiResponse.h"
#include "core/HttpServiceResponse.h"
#include "TrialsClient.h"
#include "game/mission/trial/TrialTypes.h"
#include "TrialsProvider.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTrialsProvided, const TArray<FReplicatableTrialDef>&);

UCLASS()
class DUNGEONS_API UTrialsProvider : public UObject {

	GENERATED_BODY()

public:
	void LoadTrials();

	FReplicatableTrialDefPack GetCachedTrials() const;
	void UnregisterListeners() const;

	FOnTrialsProvided OnDailyTrialsProvided;
protected:
	virtual std::shared_ptr<TrialsClient> GetTrialsClient() const;

	void OnTrialsLoaded(minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse>& response);

	void OnTrialsLoadFailed();

	UPROPERTY()
	FReplicatableTrialDefPack CachedTrialDefs;

	FDelegateHandle OnTrialsLoadedHandle;
	FDelegateHandle OnTrialsLoadFailedHandle;
};
