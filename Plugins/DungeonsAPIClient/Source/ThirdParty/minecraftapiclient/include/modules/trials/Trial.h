#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "core/Date.h"
#include <string>
#include "Rules.h"

namespace minecraft {
namespace api {
    struct Trial {
        shared_ptr<string> id;
    	shared_ptr<string> type;
        shared_ptr<Date> startDate;
        shared_ptr<Date> endDate;
        shared_ptr<bool> completed;
        shared_ptr<string> mission;
        shared_ptr<int> extraChallenge;
        shared_ptr<string> missionGroup;
        shared_ptr<Rules> rules;
    	shared_ptr<string> theme;

        void toJson(Json::Value &j) {
            writeJson(j, "id", this->id);
        	writeJson(j, "type", this->type);
            writeJson(j, "startDate", this->startDate);
            writeJson(j, "endDate", this->endDate);
            writeJson(j, "completed", this->completed);
            writeJson(j, "mission", this->mission);
            writeJson(j, "extraChallenge", extraChallenge);
            writeJson(j, "missionGroup", missionGroup);
            writeJson(j, "rules", this->rules);
        	writeJson(j, "theme", this->theme);
        }

        void fromJson(const Json::Value& j) {
            this->id = parseJson<string>(j, "id");
        	this->type = parseJson<string>(j, "type");
            this->startDate = parseJson<Date>(j, "startDate");
            this->endDate = parseJson<Date>(j, "endDate");
            this->completed = parseJson<bool>(j, "completed");
            this->mission = parseJson<string>(j, "mission");
            this->extraChallenge = parseJson<int>(j, "extraChallenge");
            this->missionGroup = parseJson<string>(j, "missionGroup");
            this->rules = parseJson<Rules>(j, "rules");
        	this->theme = parseJson<string>(j, "theme");
        }
    };
}
}
