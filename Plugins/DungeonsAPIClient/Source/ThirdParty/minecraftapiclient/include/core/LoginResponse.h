#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <Anticheat.hpp>

namespace minecraft {
namespace api {
    struct LoginResponse {
        shared_ptr<string> username;
        shared_ptr<string> accessToken;
        shared_ptr<string> liveOpsApiVersion;
        shared_ptr<int32> expiresIn;

        void toJson(Json::Value &j) {
        	ANTICHEAT_PROTECT_STRINGS_BEGIN;
            writeJson(j, "username", this->username);
            writeJson(j, "access_token", this->accessToken);
            writeJson(j, "minimumClientLiveOpsVersion", this->liveOpsApiVersion);
            writeJson(j, "expires_in", this->expiresIn);
        	ANTICHEAT_PROTECT_STRINGS_END;
        }

        void fromJson(const Json::Value& j) {
        	ANTICHEAT_PROTECT_STRINGS_BEGIN;
            this->username = parseJson<string>(j, "username");
            this->accessToken = parseJson<string>(j, "access_token");
            this->liveOpsApiVersion = parseJson<string>(j, "minimumClientLiveOpsVersion");
            this->expiresIn = parseJson<int32>(j, "expires_in");
        	ANTICHEAT_PROTECT_STRINGS_END;
        }
    };
}
}
