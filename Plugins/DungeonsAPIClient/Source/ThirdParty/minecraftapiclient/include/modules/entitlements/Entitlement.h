#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <string>

namespace minecraft {
namespace api {
    struct Entitlement {
        shared_ptr<string> name;
        shared_ptr<string> signature;
        shared_ptr<string> gameReference;

        void toJson(Json::Value &j) {
            writeJson(j, "name", this->name);
            writeJson(j, "signature", this->signature);
            writeJson(j, "gameReference", this->gameReference);
        }

        void fromJson(const Json::Value& j) {
            this->name = parseJson<string>(j, "name");
            this->signature = parseJson<string>(j, "signature");
            this->gameReference = parseJson<string>(j, "gameReference");
        }
    };
}
}
