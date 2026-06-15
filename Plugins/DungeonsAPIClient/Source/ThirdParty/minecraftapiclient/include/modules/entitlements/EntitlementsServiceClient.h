#pragma once

#include "core/HttpServiceClient.h"
#include "EntitlementsResponse.h"
#include "EntitlementsRequest.h"
#include <string>
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        class EntitlementsServiceClient {
        public:
            explicit EntitlementsServiceClient(shared_ptr<HttpServiceClient> client, shared_ptr<HttpHeaders> headers)
                    : client(move(client)), headers(move(headers)) {}

            void getEntitlements(const string& requestId, const function<void(HttpServiceResponse<EntitlementsResponse>)> &callback);
            void postEntitlements(const string& licenseJwt, const string& requestId, const function<void(HttpServiceResponse<EntitlementsResponse>)> &callback);

        private:
            shared_ptr<HttpServiceClient> client;
            shared_ptr<HttpHeaders> headers;
        };

    	ANTICHEAT_NO_OPTIMIZATION_BEGIN;
        inline void EntitlementsServiceClient::getEntitlements(const string& requestId, const function<void(HttpServiceResponse<EntitlementsResponse>)> &callback) {
            headers->fetchHeaders([=](auto &requestHeaders) {
            	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
				string path("/entitlements?requestId={requestId}");
                replace(path, "requestId", requestId);
                return client->request<EntitlementsResponse>("GET", path, requestHeaders, callback);
            	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
            });
        }

        inline void EntitlementsServiceClient::postEntitlements(const string& licenseJwt, const string& requestId, const function<void(HttpServiceResponse<EntitlementsResponse>)> &callback) {
            headers->fetchHeaders([=](const auto &requestHeaders) {
            	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
                string path("/entitlements?requestId={requestId}");
                replace(path, "requestId", requestId);

                EntitlementsRequest body(licenseJwt);
                return client->request<EntitlementsResponse>("POST", path, requestHeaders, body, callback);
            	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
            });
        }
    	
    	ANTICHEAT_NO_OPTIMIZATION_END;
    }
}
