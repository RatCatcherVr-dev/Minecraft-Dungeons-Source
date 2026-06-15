#pragma once

#include "CoreMinimal.h"
#include "core/HttpServiceResponse.h"
#include "modules/entitlements/EntitlementsResponse.h"
#include "modules/entitlements/ValidatedEntitlement.h"
#include "MinecraftAPIAuthData.h"
#include "XAuthData.h"
#include "EntitlementsRepository.h"
#include "APIEntitlementRepositoryParent.generated.h"

UCLASS()
class DUNGEONS_API UAPIEntitlementRepositoryParent : public UEntitlementsRepository {
	GENERATED_BODY()

public:
	virtual void RequestEntitlements() {}
protected:

	UEntitlementsValidator* CreateValidator() override;
	
	void ConfigureForPlatform() override;
	void OnEntitlementsLoaded(minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse>&, const FString& requestId);

	void ParseEntitlements(const vector<minecraft::api::ValidatedEntitlement>&);
	
	void OnEntitlementsLoadFailed() const;

	void RemoveDelegateBindings() const;

	void OnSuccessfulMinecraftAPIAuthentication(const MinecraftAPIAuthData&, const XAuthData&);
	void OnLogoutMinecraftAPI();

	TOptional<XAuthData> AuthData;
    TOptional<MinecraftAPIAuthData> MinecraftAuthData;

	FDelegateHandle OnEntitlementsLoadedHandle;
	FDelegateHandle OnEntitlementsLoadFailedHandle;
};