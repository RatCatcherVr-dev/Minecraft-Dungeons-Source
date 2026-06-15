#include "AuthListener.h"
#include "ClientStringUtil.h"

void AuthListener::loginSuccessful(const string& accessToken, const string& userId, const shared_ptr<minecraft::api::DungeonsData>& dungeonsData) {
	using namespace dungeonsapiclient::utils;

	FString requestId;
	TArray<FString> productIdList;

	if (dungeonsData != nullptr && dungeonsData->licenseData != nullptr) {
		if (dungeonsData->licenseData->requestId != nullptr) {
			requestId = toFString(*(dungeonsData->licenseData->requestId));
		}

		if (dungeonsData->licenseData->productIds != nullptr) {
			for (const auto& productId : *(dungeonsData->licenseData->productIds)) {
				productIdList.Emplace(toFString(productId));
			}
		}
	}

	const auto authData = MinecraftAPIAuthData(toFString(accessToken), toFString(userId), requestId, productIdList);
	OnSuccessfulLogin.Broadcast(authData);
}

void AuthListener::loginFailed() {
    OnFailedLogin.Broadcast();
}
