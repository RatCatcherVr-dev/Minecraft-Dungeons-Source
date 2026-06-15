#pragma once

#include <utility>

#include "Config.h"
#include "ClientInfo.h"
#include "HttpServiceClient.h"
#include "AuthenticationListener.h"
#include "LoginRequest.h"
#include "AuthenticationServiceClient.h"
#include "HttpHeaders.h"
#include "Authenticator.h"
#include "TimerManager.h"
#include "XAuthData.h"

namespace minecraft {
    namespace api {
        class MinecraftClient {
        public:
            explicit MinecraftClient(
                    shared_ptr<HttpClient> client,
                    ClientInfo clientInfo,
					FTimerManager& timerManager,
                    Config config = Config::createForProd())
                : config(config), clientInfo(std::move(clientInfo)), client(client), timerManager(&timerManager) {
                this->headers = make_shared<HttpHeaders>(clientInfo);
                auto httpServiceClient = make_shared<HttpServiceClient>(config.getApiEndpoint(), client);
                auto authClient = make_shared<AuthenticationServiceClient>(httpServiceClient);
                this->authenticator = make_shared<Authenticator>(authClient, headers);
            }

            const string& getGameVersion() const;
            const string& getRequiredLiveOpsGameVersion() const;
            const Config& getConfig();

            template<typename T>
            shared_ptr<T> getServiceClient();
            void dungeonsLogin(const XAuthData& authData, shared_ptr<AuthenticationListener> listener);
			FTimerManager* getTimerManager();

        private:
            Config config;
            ClientInfo clientInfo;
            shared_ptr<HttpClient> client;
            shared_ptr<HttpHeaders> headers;
            shared_ptr<Authenticator> authenticator;
			FTimerManager* timerManager;
        };

        inline const string& MinecraftClient::getRequiredLiveOpsGameVersion() const {
            return authenticator->getLiveOpsApiVersion();
        }

        inline const string& MinecraftClient::getGameVersion() const {
			return clientInfo.getClientVersion();
        }

        template<typename T>
        inline shared_ptr<T> MinecraftClient::getServiceClient() {
            auto httpServiceClient = make_shared<HttpServiceClient>(config.getApiEndpoint(), client);
            return make_shared<T>(httpServiceClient, headers);
        }

        inline const Config& MinecraftClient::getConfig() {
            return config;
        }

        inline void MinecraftClient::dungeonsLogin(const XAuthData& authData, shared_ptr<AuthenticationListener> listener) {
            authenticator->dungeonsLogin(authData, move(listener));
        }

    	inline FTimerManager* MinecraftClient::getTimerManager() {
            return timerManager;
        }
    }
}
