#pragma once

#include "Types.h"

namespace minecraft {
    namespace api {
        struct Datetime {
            timePoint timeMillis;

            Datetime() {
                timeMillis = currentTimeMillis();
            }

            void toJson(Json::Value &j) {
                j = toIsoExtendedString(this->timeMillis);
            }

            void fromJson(const Json::Value& j) {
                auto datetimeString = j.asString();
                this->timeMillis = parseIsoExtendedString(datetimeString);
            }
        };

        inline std::ostream& operator<<(std::ostream &strm, const Datetime &datetime) {
            return strm << toIsoExtendedString(datetime.timeMillis);
        } 
    }
}

