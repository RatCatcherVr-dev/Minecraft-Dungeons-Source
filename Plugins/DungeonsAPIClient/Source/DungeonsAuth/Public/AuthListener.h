#pragma once

#include "CoreMinimal.h"
#include "core/AuthenticationListener.h"
#include "core/MinecraftClient.h"
#include "MinecraftAPIAuthData.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSuccessfulLogin, MinecraftAPIAuthData);
DECLARE_MULTICAST_DELEGATE(FFailedLogin);

class AuthListener final : public minecraft::api::AuthenticationListener {

public:
    void loginSuccessful(const string& accessToken, const string& userId, const shared_ptr<minecraft::api::DungeonsData>& dungeonsData) override;

    void loginFailed() override;

	FSuccessfulLogin OnSuccessfulLogin;
	FFailedLogin OnFailedLogin;
};
