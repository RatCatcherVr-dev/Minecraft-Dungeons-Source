#include "CoreMinimal.h"
#include "EntitlementsClient.h"
#include "IDungeonsAuth.h"
#include "modules/entitlements/EntitlementsResponse.h"
#include "modules/entitlements/EntitlementsServiceClient.h"
#include "modules/entitlements/EntitlementsValidator.h"
#include "ClientStringUtil.h"
#include <Anticheat.hpp>

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
#include <intrin.h>
#elif defined(__ORBIS__)
#include <x86intrin.h>
#endif

#include <string>
#include "SecureHash.h"

#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
#include "GDKDungeons.h"
#endif

DEFINE_LOG_CATEGORY(LogMinecraftAPIEntitlements)


namespace online { namespace entitlements {
ANTICHEAT_NO_OPTIMIZATION_BEGIN
const FString requestIdFromRequest(const EntitlementsRequest& request) {
	ANTICHEAT_VIRT_BEGIN
	
#if PLATFORM_WINDOWS
	const auto uniqueId = dungeonsapiclient::utils::toFString(std::to_string(__rdtsc()));
#else
	const auto uniqueId = FGuid::NewGuid().ToString();
#endif
	const auto requestIdInput = uniqueId + request.UserId;
	return FMD5::HashAnsiString(*requestIdInput);

	ANTICHEAT_VIRT_END
}

std::string computeChallenge(const FString& computeValue) {
	ANTICHEAT_VIRT_BEGIN
	char salt[17];
	salt[0] = 'd';
	salt[1] = 'W';
	salt[2] = 'Q';
	salt[3] = '7';
	salt[4] = 'L';
	salt[5] = 'C';
	salt[6] = '6';
	salt[7] = '!';
	salt[8] = 'F';
	salt[9] = 'Q';
	salt[10] = 'B';
	salt[11] = '0';
	salt[12] = 'r';
	salt[13] = 'w';
	salt[14] = 'Q';
	salt[15] = '4';
	salt[16] = '\0';

	const auto saltedValue = computeValue + salt;
	const auto hashedString = FMD5::HashAnsiString(*saltedValue);;
	return dungeonsapiclient::utils::toString(hashedString.Mid(5, 16));

	ANTICHEAT_VIRT_END
}
}}

void EntitlementsClient::Request(const EntitlementsRequest& request) {
	ANTICHEAT_VIRT_BEGIN
	
	if (const auto client = GetAuthenticatedClient()) {		
		const auto& requestId = online::entitlements::requestIdFromRequest(request);
		const auto onResponse = [=](minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse> response) {
			ANTICHEAT_VIRT_BEGIN
			OnEntitlementsLoaded.Broadcast(response, requestId);
			ANTICHEAT_VIRT_END
		};

		client->getServiceClient<minecraft::api::EntitlementsServiceClient>()->getEntitlements(
			dungeonsapiclient::utils::toString(requestId),
			onResponse);
	} else {
		UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Unable to send entitlements request. No authenticated client available."));
		OnEntitlementsLoadFailed.Broadcast();
	}

	ANTICHEAT_VIRT_END
}

void EntitlementsClient::Request(const EntitlementsRequest& request, const FString& challenge, const TArray<FString>& productIdList) {
	ANTICHEAT_VIRT_BEGIN
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1

		if (const auto client = GetAuthenticatedClient()) {
			const auto& requestId = online::entitlements::requestIdFromRequest(request);
			const auto onResponse = [=](minecraft::api::HttpServiceResponse<minecraft::api::EntitlementsResponse> response) {
				ANTICHEAT_VIRT_BEGIN
					OnEntitlementsLoaded.Broadcast(response, requestId);
				ANTICHEAT_VIRT_END
			};

			auto& gdk = FGDKDungeonsInterface::Get();
			HRESULT hr = gdk.InitializeGDK();
			UE_CLOG(FAILED(hr), LogMinecraftAPIEntitlements, Warning, TEXT("Failed to initialize GDK in entitlements client, hr 0x%x"), hr);
			if (SUCCEEDED(hr)) {
				hr = gdk.CreateStoreContext();
				UE_CLOG(FAILED(hr), LogMinecraftAPIEntitlements, Warning, TEXT("Failed to create store context in entitlements client, hr 0x%x"), hr);
				if (SUCCEEDED(hr)) {
					vector<string> productIdStrings;
					vector<const char*> productIds;
					productIdStrings.reserve(productIdList.Num());
					productIds.reserve(productIdList.Num());

					for (size_t i = 0; i < productIdList.Num(); i++) {
						productIdStrings.push_back(TCHAR_TO_ANSI(*productIdList[i]));
						productIds.push_back(productIdStrings[i].c_str());
					}

					hr = gdk.QueryLicenseTokenAsync(
						productIds.data(),
						productIds.size(),
						TCHAR_TO_ANSI(*challenge),
						[=](std::string token) {
							ANTICHEAT_VIRT_BEGIN
							if (token.empty()) {
								UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Empty token received during licensing"));
								OnEntitlementsLoadFailed.Broadcast();
							}
							else {
								client->getServiceClient<minecraft::api::EntitlementsServiceClient>()->postEntitlements(
									token,
									dungeonsapiclient::utils::toString(requestId),
									onResponse);
							}
							ANTICHEAT_VIRT_END
						}
					);
					UE_CLOG(FAILED(hr), LogMinecraftAPIEntitlements, Warning, TEXT("Failed to query license token, hr 0x%x"), hr);
				}
			}

			if (FAILED(hr)) {
				UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Failure during token licensing."));
				OnEntitlementsLoadFailed.Broadcast();
			}
		}
		else {
			UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Unable to send entitlements request. No authenticated client available."));
			OnEntitlementsLoadFailed.Broadcast();
		}
#endif
	ANTICHEAT_VIRT_END
}

vector<minecraft::api::ValidatedEntitlement> EntitlementsClient::Validate(const minecraft::api::EntitlementsResponse& response, const FString& userId, const FString& requestId) {
	ANTICHEAT_VIRT_BEGIN

	if (auto validator = GetValidator()) {
		return validator->validate(response, dungeonsapiclient::utils::toString(userId), online::entitlements::computeChallenge(requestId));
	}

	UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Unable to validate entitlements response. No authenticated client available."));

	return {};

	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_END

vector<minecraft::api::ValidatedEntitlement> EntitlementsClient::Validate(const FString& signature, const FString& userId) {
	if (auto validator = GetValidator()) {
		return validator->validate(dungeonsapiclient::utils::toString(signature), dungeonsapiclient::utils::toString(userId));	
	}

	UE_LOG(LogMinecraftAPIEntitlements, Warning, TEXT("Unable to validate entitlement signature. No authenticated client available."));
	
	return {};
}

shared_ptr<minecraft::api::MinecraftClient> EntitlementsClient::GetAuthenticatedClient() {
	return IDungeonsAuth::Get().Auth() ? IDungeonsAuth::Get().Auth()->GetClient() : std::shared_ptr<minecraft::api::MinecraftClient>(nullptr);
}

TOptional<minecraft::api::EntitlementsValidator> EntitlementsClient::GetValidator() {
	if (const auto client = GetAuthenticatedClient()) {
		return minecraft::api::EntitlementsValidator(client->getConfig());
	}

	return TOptional<minecraft::api::EntitlementsValidator>();
}

