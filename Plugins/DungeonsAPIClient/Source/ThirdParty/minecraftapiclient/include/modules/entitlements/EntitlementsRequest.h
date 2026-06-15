#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"

namespace minecraft {
    namespace api {
        struct EntitlementsRequest {
            shared_ptr<string> licenseJwt;

            explicit EntitlementsRequest(const string& licenseJwt) {
                this->licenseJwt = std::make_shared<string>(licenseJwt);
            }

            void toJson(Json::Value &j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                writeJson(j, "licenseJwt", this->licenseJwt);
            	ANTICHEAT_PROTECT_STRINGS_END;
            }

            void fromJson(const Json::Value& j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->licenseJwt = parseJson<string>(j, "licenseJwt");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };
    }
}