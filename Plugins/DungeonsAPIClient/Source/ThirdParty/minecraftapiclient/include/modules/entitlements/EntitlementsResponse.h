#pragma once

#include "Entitlement.h"
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        struct EntitlementsResponse {
            shared_ptr<vector<Entitlement>> items;
            shared_ptr<string> signature;

            void toJson(Json::Value &j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                writeArrayJson(j, "items", this->items);
                writeJson(j, "signature", this->signature);
            	ANTICHEAT_PROTECT_STRINGS_END;
            }

            void fromJson(const Json::Value& j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->items = parseArrayJson<Entitlement>(j, "items");
                this->signature = parseJson<string>(j, "signature");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };
    }
}