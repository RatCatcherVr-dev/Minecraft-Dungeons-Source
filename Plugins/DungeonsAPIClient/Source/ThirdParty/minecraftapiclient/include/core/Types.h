#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define MINECRAFT_API_WINDOWS
#endif

// c++ standard template library
#include <cstdint>
#include <type_traits>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <regex>
#include <functional>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

using std::is_same;
using std::string;
using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::move;
using std::unordered_map;
using std::function;

// json parsing/writing
#include <json/json.h>

// date parsing
typedef std::chrono::system_clock::time_point timePoint;


//
// Utility functions
//


namespace minecraft {
    namespace api {
        inline void replace(string& original, const string &match, const string &replace) {
            std::regex regExp("\\{" + match + "\\}");
            original = std::regex_replace(original, regExp, replace, std::regex_constants::format_first_only); 
        }

        inline timePoint parseDateTimeFormat(const string &format, const std::string &datetime) {
            std::tm tm = {};
            std::istringstream in(datetime);
            in >> std::get_time(&tm, format.c_str());
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }

        inline timePoint parseIsoExtendedString(const std::string &str) {
            return parseDateTimeFormat("%FT%TZ", str);
        }

        inline std::tm getLocaltime(time_t t) {
#ifdef MINECRAFT_API_WINDOWS
            std::tm new_tm{};
            localtime_s(&new_tm, &t);
#else
            std::tm new_tm = *std::localtime(&t);
#endif
            return new_tm;
        }

        inline string toFormattedString(const timePoint& timeMillis, std::string format) {
            std::time_t t = std::chrono::system_clock::to_time_t(timeMillis);
            std::tm new_tm = getLocaltime(t);
            std::ostringstream ss;
            ss << std::put_time(&new_tm, format.c_str());
            return ss.str();
        }

        inline string toIsoExtendedString(const timePoint &timeMillis) {
            return toFormattedString(timeMillis, "%FT%TZ");
        }

        inline timePoint parseYearMonthDayString(const std::string & str) {
            return parseDateTimeFormat("%Y-%m-%d", str);
        }

        inline timePoint parseDateTimeString(const std::string & str) {
            return parseDateTimeFormat("%Y-%m-%dT%H:%M:%S", str);
        }

        inline string toYearMonthDayString(const timePoint &timeMillis) {
            return toFormattedString(timeMillis, "%Y-%m-%d");
        }

        inline string toDateTimeString(const timePoint &timeMillis) {
            return toFormattedString(timeMillis, "%Y-%m-%dT%H:%M:%S");
        }

        inline timePoint currentTimeMillis() {
            return std::chrono::system_clock::now();
        }
    }
}