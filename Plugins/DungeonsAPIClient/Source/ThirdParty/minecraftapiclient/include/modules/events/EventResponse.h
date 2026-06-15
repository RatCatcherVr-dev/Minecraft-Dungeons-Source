#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "Event.h"
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        struct EventsResponse {
            shared_ptr<string> status;

            void toJson(Json::Value &j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                writeJson(j, "status", this->status);
            	ANTICHEAT_PROTECT_STRINGS_END;
            }

            void fromJson(const Json::Value& j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                this->status = parseJson<string>(j, "status");
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };
    }
}