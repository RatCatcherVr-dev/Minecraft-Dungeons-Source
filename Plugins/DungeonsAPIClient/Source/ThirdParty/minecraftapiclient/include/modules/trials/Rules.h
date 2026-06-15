#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "Rule.h"
#include <vector>

namespace minecraft {
namespace api {
    struct Rules {
        shared_ptr<vector<Rule>> modifiers;
        shared_ptr<vector<Rule>> rewards;

        void toJson(Json::Value &j) {
            writeArrayJson(j, "modifiers", this->modifiers);
            writeArrayJson(j, "rewards", this->rewards);
        }

        inline void fromJson(const Json::Value& j) {
            this->modifiers = parseArrayJson<Rule>(j, "modifiers");
            this->rewards = parseArrayJson<Rule>(j, "rewards");
        }
    };
}
}
