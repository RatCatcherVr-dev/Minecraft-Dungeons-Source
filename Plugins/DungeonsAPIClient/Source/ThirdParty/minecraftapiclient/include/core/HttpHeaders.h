#pragma once

#include "Types.h"
#include "HttpClient.h"
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        class HttpHeaders {
        public:
            explicit HttpHeaders(const ClientInfo& clientInfo) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                add("Content-Type", "application/json");
				add("X-Client-Name", clientInfo.getClientName());
				add("X-Client-Version", clientInfo.getClientVersionFull());
				add("X-Client-Build", clientInfo.getClientBuild());
            	ANTICHEAT_PROTECT_STRINGS_END;
            }

            void add(const string& key, const string& value) {
                headers[key] = {key, value};
            }

            vector<HttpHeader> getHeaders() {
                std::vector<HttpHeader> resultHeaders;
                resultHeaders.reserve(headers.size());

                auto getSecond = [](auto pair) {return pair.second;};
                std::transform (headers.begin(), headers.end(), back_inserter(resultHeaders), getSecond);

                return resultHeaders;
            }

            void fetchHeaders(const function<void(vector<HttpHeader>& headers)>& callback) {
                auto requestHeaders = getHeaders();
                callback(requestHeaders);
            }

            void updateAuthToken(const string& authToken) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                add("Authorization", "Bearer " + authToken);
            	ANTICHEAT_PROTECT_STRINGS_END;
            }

        private:
            unordered_map<string, HttpHeader> headers;
        };
    }
}