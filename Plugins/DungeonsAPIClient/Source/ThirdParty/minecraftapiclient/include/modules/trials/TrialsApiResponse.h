#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "Trial.h"
#include <Anticheat.hpp>

namespace minecraft {
namespace api {
    struct TrialsApiResponse {
        shared_ptr<vector<Trial>> trials;

        void toJson(Json::Value& j) {
        	ANTICHEAT_PROTECT_STRINGS_BEGIN;
            writeArrayJson(j, "trials", this->trials);
        	ANTICHEAT_PROTECT_STRINGS_END;
        }

        void fromJson(const Json::Value& j) {
        	ANTICHEAT_PROTECT_STRINGS_BEGIN;
            this->trials = parseArrayJson<Trial>(j, "trials");
        	ANTICHEAT_PROTECT_STRINGS_END;
        }
    };
}
}
