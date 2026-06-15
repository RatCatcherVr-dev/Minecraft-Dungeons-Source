#include "DungeonsAuthenticator.h"
#include "AuthListener.h"
#include "ClientStringUtil.h"
#include "IDungeonsHTTP.h"
#include "AuthCommon.h"
#include "MinecraftAPIAuthData.h"
#include "IDungeonsClient.h"

DungeonsAuthenticator::DungeonsAuthenticator()
	: DungeonsAuthListener(std::make_shared<AuthListener>()) {
}

void DungeonsAuthenticator::Authenticate(const XAuthData& xauthData) {
	if (!DungeonsAuthListener->OnSuccessfulLogin.IsBoundToObject(this)) {
		const auto minecraftClient = IDungeonsClient::Get().CreateMinecraftClient();

		SuccessfulLoginHandle = DungeonsAuthListener->OnSuccessfulLogin.AddRaw(this, &DungeonsAuthenticator::OnSuccessfulLogin, xauthData, minecraftClient);
		FailedLoginHandle = DungeonsAuthListener->OnFailedLogin.AddRaw(this, &DungeonsAuthenticator::OnFailedLogin, xauthData);

		AuthenticateClient(minecraftClient, xauthData);
	}
}

void DungeonsAuthenticator::AuthenticateClient(shared_ptr<minecraft::api::MinecraftClient> minecraftClient, const XAuthData& xauthData) const {
	minecraftClient->dungeonsLogin(xauthData, DungeonsAuthListener);
}

std::shared_ptr<minecraft::api::MinecraftClient> DungeonsAuthenticator::GetClient() const {
	return AuthenticatedMinecraftClient;
}

void DungeonsAuthenticator::OnSuccessfulLogin(const MinecraftAPIAuthData apiData, const XAuthData xauthData, const shared_ptr<minecraft::api::MinecraftClient> minecraftClient) {
	AuthenticatedMinecraftClient = minecraftClient;
	OnClientSuccessfullyAuthenticated.Broadcast(apiData, xauthData);
	RemoveDelegateBindings();
}

void DungeonsAuthenticator::OnFailedLogin(const XAuthData xauthData) {
	AuthenticatedMinecraftClient = nullptr;
	OnClientAuthenticationFailed.Broadcast(xauthData);
	RemoveDelegateBindings();
}

void DungeonsAuthenticator::RemoveDelegateBindings() const {
	DungeonsAuthListener->OnSuccessfulLogin.Remove(SuccessfulLoginHandle);
	DungeonsAuthListener->OnFailedLogin.Remove(FailedLoginHandle);
}

void DungeonsAuthenticator::Retry() {
	OnRetryClientAuthentication.Broadcast();
}

void DungeonsAuthenticator::Logout() {
	AuthenticatedMinecraftClient = nullptr;
	OnLogoutClientAuthentication.Broadcast();
}

