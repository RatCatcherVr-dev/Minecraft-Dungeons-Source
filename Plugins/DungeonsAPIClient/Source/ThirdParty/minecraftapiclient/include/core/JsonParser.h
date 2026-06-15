#pragma once

#include "Types.h"

namespace minecraft {
    namespace api {
        struct JsonParseResult {
            bool parsed{};
            Json::Value jsonValue;
        };

        class JsonParser {
        public:
            JsonParser() = default;

            JsonParseResult parse(const string& jsonText);
            string write(const Json::Value& jsonObject);

        private:
            Json::FastWriter writer;
        };

        inline JsonParseResult JsonParser::parse(const string& jsonText) {
            JsonParseResult parseResult;

            if (jsonText.empty()) {
                parseResult.parsed = false;
                return parseResult;
            }

            Json::Reader reader;
            parseResult.parsed = reader.parse(jsonText, parseResult.jsonValue);
            return parseResult;
        }

        inline string JsonParser::write(const Json::Value& jsonObject) {
            return writer.write(jsonObject);
        }
    }
}