#pragma once

#include "AuthenticationListener.h"
#include "AuthenticationServiceClient.h"
#include "HttpHeaders.h"
#include <Anticheat.hpp>
#include "XAuthData.h"
#include "ClientStringUtil.h"

namespace minecraft {
    namespace api {

		inline std::string GetServicesPlatformType() {
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
			return "onestore";
#elif defined(STEAM_BUILD)
			return "steam";
#elif PLATFORM_WINDOWS
			return "pc_launcher";
#elif PLATFORM_XBOXONE
			return "xbox";
#elif PLATFORM_SWITCH
			return "switch";
#elif PLATFORM_PS4
			return "playstation";
#else
			static_assert(false, "No platform specified");
#endif
		}

        class AccessTokenData {
        public:
            AccessTokenData(string  accessToken, string userId)
                    : accessToken(move(accessToken)), userId(move(userId)) {
            }

            static shared_ptr<AccessTokenData> createNullObject() {
                return make_shared<AccessTokenData>("", "");
            }

            const string& getAccessToken() { return accessToken; }
            const string& getUserId() { return userId; }

        private:
            string accessToken;
            string userId;
        };

        class Authenticator {
        public:
            explicit Authenticator(shared_ptr<AuthenticationServiceClient> authClient, shared_ptr<HttpHeaders> httpHeaders)
                    : accessTokenData(AccessTokenData::createNullObject()), authClient(move(authClient)), httpHeaders(move(httpHeaders)) {}

            void dungeonsLogin(const XAuthData& authData, shared_ptr<AuthenticationListener> listener) {
				ANTICHEAT_OBFUSCATE_BEGIN
				authListener = move(listener);
				LoginRequest loginRequest(
					"PLAYFAB",
					GetServicesPlatformType(),
					dungeonsapiclient::utils::toString(authData.PlayfabToken),
					dungeonsapiclient::utils::toString(authData.PlatformToken),
					dungeonsapiclient::utils::toString(authData.NamespaceLogin)
				);
				authClient->dungeonsLogin(loginRequest, [&](auto response) {
					handleLoginResponse(response);
				});
            	ANTICHEAT_OBFUSCATE_END;
            }

            void logout() {
                accessTokenData = AccessTokenData::createNullObject();
            }

            const string& getLiveOpsApiVersion() const {
                return liveOpsApiVersion;
            }

        private:
            template<typename T>
            void handleLoginResponse(HttpServiceResponse<T> response) {
                if (response.successful()) {
                    saveAccessTokenData(response.getBody());
                    saveLiveOpsApiVersion(response.getBody());
                    httpHeaders->updateAuthToken(accessTokenData->getAccessToken());
                    authListener->loginSuccessful(accessTokenData->getAccessToken(), accessTokenData->getUserId(), nullptr);
                } else {
                    authListener->loginFailed();
                }
            }

            template<>
            void handleLoginResponse(HttpServiceResponse<DungeonsLoginResponse> dungeonsLoginResponse) {
                if (dungeonsLoginResponse.successful()) {
                    const auto& body = dungeonsLoginResponse.getBody();
                    saveAccessTokenData(body->loginResponse);
                    saveLiveOpsApiVersion(body->loginResponse);
                    httpHeaders->updateAuthToken(accessTokenData->getAccessToken());
                    authListener->loginSuccessful(accessTokenData->getAccessToken(), accessTokenData->getUserId(), body->dungeonsData);
                } else {
                    authListener->loginFailed();
                }
            }

            void saveAccessTokenData(const shared_ptr<LoginResponse>& response) {
                this->accessTokenData = make_shared<AccessTokenData>(
                        *response->accessToken,
                        *response->username);
            }

            void saveLiveOpsApiVersion(const shared_ptr<LoginResponse>& response) {
				this->liveOpsApiVersion = *response->liveOpsApiVersion;
            }

        private:
            shared_ptr<AccessTokenData> accessTokenData;
            shared_ptr<AuthenticationServiceClient> authClient;
            shared_ptr<HttpHeaders> httpHeaders;
            shared_ptr<AuthenticationListener> authListener;
            string liveOpsApiVersion;
        };
    }
}