#pragma once

#include "Types.h"
#include "JsonSerializer.h"
#include "LoginResponse.h"
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        struct LicenseData {
            shared_ptr<string> requestId;
            shared_ptr<vector<string>> productIds;

            void fromJson(const Json::Value& j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->requestId = parseJson<string>(j, "requestId");
                this->productIds = parseArrayJsonRaw(j, "productIds");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };

        struct DungeonsData {
            shared_ptr<LicenseData> licenseData;

            void fromJson(const Json::Value& j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->licenseData = parseJson<LicenseData>(j, "licenseData");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };

        struct DungeonsLoginResponse {
            shared_ptr<LoginResponse> loginResponse;
            shared_ptr<DungeonsData> dungeonsData;

            DungeonsLoginResponse() {
                loginResponse = std::make_shared<LoginResponse>();
            }

            void fromJson(const Json::Value& j) {
                loginResponse->fromJson(j);
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->dungeonsData = parseJson<DungeonsData>(j, "dungeonsData");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };
    }
}