#pragma once

#include "CoreMinimal.h"
#include "core/HttpServiceResponse.h"
#include "core/MinecraftClient.h"
#include "modules/entitlements/EntitlementsResponse.h"
#include "modules/entitlements/ValidatedEntitlement.h"
#include "modules/entitlements/EntitlementsValidator.h"
#include "EntitlementsRequest.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEntitlementsLoaded, minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse>&, const FString&/* requestId*/)
DECLARE_MULTICAST_DELEGATE(FOnEntitlementsLoadFailed)

DECLARE_LOG_CATEGORY_EXTERN(LogMinecraftAPIEntitlements, Log, All)

class DUNGEONSENTITLEMENTS_API EntitlementsClient {
public:
	void Request(const EntitlementsRequest&);
	void Request(const EntitlementsRequest& request, const FString& challenge, const TArray<FString>& productIdList);

	vector<minecraft::api::ValidatedEntitlement> Validate(const minecraft::api::EntitlementsResponse&, const FString& userId, const FString& requestId);
	vector<minecraft::api::ValidatedEntitlement> Validate(const FString& signature, const FString& userId);

	FOnEntitlementsLoaded OnEntitlementsLoaded;
	FOnEntitlementsLoadFailed OnEntitlementsLoadFailed;
private:	
	shared_ptr<minecraft::api::MinecraftClient> GetAuthenticatedClient();
	TOptional<minecraft::api::EntitlementsValidator> GetValidator();
};
