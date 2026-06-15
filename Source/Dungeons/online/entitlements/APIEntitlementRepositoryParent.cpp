#include "Dungeons.h"
#include "APIEntitlementRepositoryParent.h"
#include "util/StringUtil.h"
#include "minecraftapi/MinecraftAPIEntitlementsValidator.h"
#include "minecraftapi/MinecraftAPIEntitlements.h"
#include "IDungeonsAPIClient.h"
#include "modules/entitlements/EntitlementsResponse.h"
#include "core/HttpServiceResponse.h"
#include "modules/entitlements/ValidatedEntitlement.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Themida/Anticheat.hpp>
#include "Entitlement.h"

namespace online { namespace entitlements { namespace minecraftapi {
	
	bool isValid(minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse>& response) {
		if (response.successful()) {
			if (response.getBody()) {
				return true;
			}
				
			UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Backend response for entitlements was empty."))
		} else {
			UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Backend response for entitlements failed with status %d."), response.getStatus());
		}	

		return false;
	}

	FEntitlement toFEntitlement(const minecraft::api::ValidatedEntitlement& entitlement, const EEntitlementsSource source) {
		return FEntitlement(stringutil::toFString(entitlement.getEntitlementName()),
                    stringutil::toFString(entitlement.getSignature()),
                    stringutil::toFString(entitlement.getSignerId()),
					source);
	}

	vector<minecraft::api::ValidatedEntitlement> validateResponse(const minecraft::api::EntitlementsResponse& responseBody, const FString userId, const FString requestId) {
		if (const auto entitlements = getEntitlementsClient()) {
			return entitlements->Validate(responseBody, userId, requestId);
		}

		UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Unable to verify loaded entitlements. DungeonsAPIClient module is unavailable."));
		return vector<minecraft::api::ValidatedEntitlement>();
	}

	TArray<FEntitlement> parseEntitlements(const vector<minecraft::api::ValidatedEntitlement>& validatedEntitlements, const EEntitlementsSource source) {
		TArray<FEntitlement> entitlements;
		for (auto& entitlement : validatedEntitlements) {
			entitlements.Add(toFEntitlement(entitlement, source));
		}
		return entitlements;
	}

}}}

void UAPIEntitlementRepositoryParent::ConfigureForPlatform() {
	if (IDungeonsAPIClient::IsAvailable()) {
		IDungeonsAPIClient::Get().Auth()->OnClientSuccessfullyAuthenticated.AddUObject(this, &UAPIEntitlementRepositoryParent::OnSuccessfulMinecraftAPIAuthentication);
		IDungeonsAPIClient::Get().Auth()->OnLogoutClientAuthentication.AddUObject(this, &UAPIEntitlementRepositoryParent::OnLogoutMinecraftAPI);
	}
}

UEntitlementsValidator* UAPIEntitlementRepositoryParent::CreateValidator() {
	return NewObject<UMinecraftAPIEntitlementsValidator>();
}

/*
	Consoles will only receive cosmetic entitlements from this repo. Therefor we should not trigger
	RequestFailed from this repo on console because it should not trigger game entitlements not available.
	We should still be able to play on console regardless of minecraft api connectivity or entitlement request failures.
*/
ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UAPIEntitlementRepositoryParent::OnEntitlementsLoaded(minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse>& response, const FString& requestId) {
	ANTICHEAT_VIRT_BEGIN
	
	if (online::entitlements::minecraftapi::isValid(response)) {
		if (AuthData) {
			const auto validatedEntitlements = online::entitlements::minecraftapi::validateResponse(*response.getBody(), AuthData->Xuid, requestId);
			ParseEntitlements(validatedEntitlements);
			OnEntitlementsProvided.Broadcast(GetEntitlements());
		} else {
			UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Missing required authenticated user data. Unable to validate received entitlements."));
			if (PLATFORM_WINDOWS) {
				OnEntitlementsRequestFailed.Broadcast();
			} else {
				OnEntitlementsProvided.Broadcast({});
			}
		}		
	} else {
		UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Received invalid response from entitlements backend service."));
		if (PLATFORM_WINDOWS) {
			OnEntitlementsRequestFailed.Broadcast();
		} else {
			OnEntitlementsProvided.Broadcast({});
		}
	}

	RemoveDelegateBindings();

	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UAPIEntitlementRepositoryParent::ParseEntitlements(const vector<minecraft::api::ValidatedEntitlement>& validatedEntitlements) {
	const auto parsedEntitlements = online::entitlements::minecraftapi::parseEntitlements(validatedEntitlements, GetSource());
	SetEntitlements(parsedEntitlements);
}

void UAPIEntitlementRepositoryParent::OnEntitlementsLoadFailed() const {
	UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Entitlements request failed."));
	OnEntitlementsRequestFailed.Broadcast();	
	RemoveDelegateBindings();
}

void UAPIEntitlementRepositoryParent::RemoveDelegateBindings() const {
	if (const auto entitlements = online::entitlements::minecraftapi::getEntitlementsClient()) {
		entitlements->OnEntitlementsLoaded.Remove(OnEntitlementsLoadedHandle);
		entitlements->OnEntitlementsLoadFailed.Remove(OnEntitlementsLoadFailedHandle);
	}
}

void UAPIEntitlementRepositoryParent::OnSuccessfulMinecraftAPIAuthentication(const MinecraftAPIAuthData& apiAuthData, const XAuthData& xauthData) {
	AuthData = xauthData;
	MinecraftAuthData = apiAuthData;
	RequestEntitlements();
}

void UAPIEntitlementRepositoryParent::OnLogoutMinecraftAPI() {
	AuthData = {};
	MinecraftAuthData = {};
}

