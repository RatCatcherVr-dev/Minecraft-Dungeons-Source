#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include <string>

namespace minecraft {
namespace api {
    struct CompletedTrialRequest {
        shared_ptr<string> trialId;
        shared_ptr<string> game;

        void toJson(Json::Value &j) {
            writeJson(j, "trialId", this->trialId);
            writeJson(j, "game", this->game);
        }

        void fromJson(const Json::Value& j) {
            this->trialId = parseJson<string>(j, "trialId");
            this->game = parseJson<string>(j, "game");
        }
    };
}
}
