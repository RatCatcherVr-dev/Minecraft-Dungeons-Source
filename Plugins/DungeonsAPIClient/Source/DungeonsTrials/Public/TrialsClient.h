#pragma once

#include "CoreMinimal.h"
#include "modules/trials/TrialsApiResponse.h"
#include "core/HttpServiceResponse.h"
#include "core/MinecraftClient.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsTrials, Log, All)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTrialsLoaded, minecraft::api::HttpServiceResponse<minecraft::api::TrialsApiResponse>&)
DECLARE_MULTICAST_DELEGATE(FOnTrialsLoadFailed)

class DUNGEONSTRIALS_API TrialsClient {
public:
	virtual ~TrialsClient() = default;

	virtual void Request();
	
	FOnTrialsLoaded OnTrialsLoaded;
	FOnTrialsLoadFailed OnTrialsLoadFailed;

protected:
	virtual shared_ptr<minecraft::api::MinecraftClient> GetClient();
	
};
