#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <unordered_map>
#include <string>

namespace minecraft {
namespace api {
    struct Rule {
        shared_ptr<unordered_map<string,string>> properties;

        void toJson(Json::Value &j) {
            writeMapJson(j, "properties", this->properties);
        }

        void fromJson(const Json::Value& j) {
            this->properties = parseMapJson(j, "properties");
        }
    };
}
}
