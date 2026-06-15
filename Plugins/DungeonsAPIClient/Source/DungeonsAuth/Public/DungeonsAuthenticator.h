#pragma once

#include "CoreMinimal.h"
#include "core/HttpClient.h"
#include "AuthListener.h"
#include "XAuthData.h"
#include "MinecraftAPIAuthData.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FSuccessfulClientAuthentication, const MinecraftAPIAuthData&, const XAuthData&);
DECLARE_MULTICAST_DELEGATE_OneParam(FFailedClientAuthentication, const XAuthData&);
DECLARE_MULTICAST_DELEGATE(FRetryClientAuthentication);
DECLARE_MULTICAST_DELEGATE(FLogoutClientAuthentication);

class DUNGEONSAUTH_API DungeonsAuthenticator {
	
public:
	DungeonsAuthenticator();

	void Authenticate(const XAuthData&);
	void Retry();

	void Logout();

	std::shared_ptr<minecraft::api::MinecraftClient> GetClient() const;

	FSuccessfulClientAuthentication OnClientSuccessfullyAuthenticated;
	FFailedClientAuthentication OnClientAuthenticationFailed;
	FRetryClientAuthentication OnRetryClientAuthentication;
	FLogoutClientAuthentication OnLogoutClientAuthentication;
private:
	void AuthenticateClient(shared_ptr<minecraft::api::MinecraftClient>, const XAuthData&) const;
	
	void OnSuccessfulLogin(MinecraftAPIAuthData, XAuthData, shared_ptr<minecraft::api::MinecraftClient>);
	void OnFailedLogin(XAuthData);

	void RemoveDelegateBindings() const;
	
	shared_ptr<minecraft::api::MinecraftClient> AuthenticatedMinecraftClient = nullptr;
	shared_ptr<AuthListener> DungeonsAuthListener = nullptr;

	FDelegateHandle SuccessfulLoginHandle;
	FDelegateHandle FailedLoginHandle;
};
