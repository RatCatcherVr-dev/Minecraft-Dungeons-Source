#pragma once

#include <string>
#include <unordered_map>
#include <json/json.h>

using std::unordered_map;

namespace minecraft {
    namespace jwt {
        class Claim {
        public:
            explicit Claim(const Json::Value& value)
                    : value(value) {}

            string asString() const {
                if (!value.isString()) {
                    throw std::bad_cast();
                }

                return value.asString();
            }

            Json::Value asArray() const {
                if (!value.isArray()) {
                    throw std::bad_cast();
                }

                return value;
            }

            static unordered_map<string, Claim> parseClaims(const string& str) {
                unordered_map<string, Claim> res;
                Json::Value jsonValue = parseJson(str);

                for (string& key : jsonValue.getMemberNames()) {
                    res.insert({ key, Claim(jsonValue[key]) });
                }

                return res;
            };

        private:
            static Json::Value parseJson(const string& jsonText) {
                Json::Value jsonValue;
                Json::Reader reader;
                bool parsed = reader.parse(jsonText, jsonValue);

                if (!parsed) {
                    throw std::runtime_error("Invalid json");
                }

                return jsonValue;
            };

        private:
            Json::Value value;
        };
    }
}