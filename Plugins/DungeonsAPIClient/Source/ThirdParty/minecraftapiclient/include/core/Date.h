#pragma once

#include "Types.h"

namespace minecraft {
    namespace api {
        struct Date {
            timePoint timeMillis;

            void toJson(Json::Value &j) {
                j = toYearMonthDayString(this->timeMillis);
            }

            void fromJson(const Json::Value& j) {
                auto dateString = j.asString();
                this->timeMillis = parseYearMonthDayString(dateString);
            }
        };

        inline std::ostream& operator<<(std::ostream &strm, const Date &date) {
            return strm << toYearMonthDayString(date.timeMillis);
        } 

        struct DateTime {
            timePoint timeMillis;

            void toJson(Json::Value &j) {
                j = toDateTimeString(this->timeMillis);
            }

            void fromJson(const Json::Value& j) {
                auto dateString = j.asString();
                this->timeMillis = parseDateTimeString(dateString);
            }
        };

        inline std::ostream& operator<<(std::ostream &strm, const DateTime &date) {
            return strm << toDateTimeString(date.timeMillis);
        } 
    }
}
