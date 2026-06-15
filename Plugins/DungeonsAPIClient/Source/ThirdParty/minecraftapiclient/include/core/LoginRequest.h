#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <Anticheat.hpp>

namespace minecraft {
namespace api {
        struct LoginRequest {
            shared_ptr<string> playfabtoken;
            shared_ptr<string> platformtoken;
            shared_ptr<string> loginType;
            shared_ptr<string> platform;
            shared_ptr<string> namespaceLogin;

            explicit LoginRequest(const string& loginType, const string& platform, const string& playfabtoken, const string& platformtoken, const string& namespaceLogin) {
                this->platformtoken = make_shared<string>(platformtoken);
                this->playfabtoken = make_shared<string>(playfabtoken);
                this->loginType = make_shared<string>(loginType);
                this->platform = make_shared<string>(platform);
                this->namespaceLogin = make_shared<string>(namespaceLogin);
            }

            void toJson(Json::Value &j) {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                writeJson(j, "playfabToken", this->playfabtoken);
#if !PLATFORM_PS4
                writeJson(j, "xtoken", this->platformtoken);
#endif
                writeJson(j, "loginType", this->loginType);
                writeJson(j, "platform", this->platform);
                writeJson(j, "namespace", this->namespaceLogin);
            	ANTICHEAT_PROTECT_STRINGS_END;
            }
        };
}
}
