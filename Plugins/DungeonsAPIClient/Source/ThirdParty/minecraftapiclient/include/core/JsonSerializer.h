#pragma once

#include "Types.h"
#include "Date.h"
#include "Datetime.h"

namespace minecraft {
    namespace api {

        // PARSE

        template<typename T>
        inline void parse_json(const Json::Value& j, T& value) {
            value.fromJson(j);
        }


        template<typename T>
        inline shared_ptr<T> parseJson(const Json::Value &j) {
            shared_ptr<T> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<T>();
                parse_json(j, *value);
            }

            return value;
        }

        template<>
        inline shared_ptr<string> parseJson(const Json::Value &j) {
            shared_ptr<string> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<string>(j.asString());
            }

            return value;
        }

        template<>
        inline shared_ptr<int32> parseJson(const Json::Value &j) {
            shared_ptr<int32> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<int32>(j.asInt());
            }

            return value;
        }

        template<>
        inline shared_ptr<int64> parseJson(const Json::Value &j) {
            shared_ptr<int64> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<int64>(j.asInt64());
            }

            return value;
        }

        template<>
        inline shared_ptr<bool> parseJson(const Json::Value &j) {
            shared_ptr<bool> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<bool>(j.asBool());
            }

            return value;
        }

        template<>
        inline shared_ptr<Date> parseJson(const Json::Value &j) {
            shared_ptr<Date> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<Date>();
                value->fromJson(j);
            }

            return value;
        }

        template<>
        inline shared_ptr<Datetime> parseJson(const Json::Value &j) {
            shared_ptr<Datetime> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<Datetime>();
                value->fromJson(j);
            }

            return value;
        }

        template<>
        inline shared_ptr<Json::Value> parseJson(const Json::Value &j) {
            shared_ptr<Json::Value> value = nullptr;
            if (!j.isNull()) {
                value = make_shared<Json::Value>(j);
            }

            return value;
        }
        
        template<typename T>
        inline shared_ptr<T> parseJson(const Json::Value &j, const string &name) {
            return parseJson<T>(j[name]);
        }

        template<typename T>
        inline shared_ptr<vector<T>> parseArrayJson(const Json::Value &arrayJson) {
            shared_ptr<vector<T>> typeArray = make_shared<vector<T>>();
            
            if (!arrayJson.isArray()) {
                return typeArray;
            }

            for (const auto &itemJson : arrayJson) {
                T type;
                parse_json(itemJson, type);
                typeArray->push_back(type);
            }

            return typeArray;
        }

        template<typename T>
        inline shared_ptr<vector<T>> parseArrayJson(const Json::Value &j, const string &name) {
            auto arrayJson = j[name];
            return parseArrayJson<T>(arrayJson);
        }

        inline shared_ptr<unordered_map<string, string>> parseMapJson(const Json::Value &mapJson) {
            shared_ptr<unordered_map<string, string>> typeMap = make_shared<unordered_map<string, string>>();
            
            if (!mapJson.isObject()) {
                return typeMap;
            }

            auto keys = mapJson.getMemberNames();
            for (auto key : keys) {
                auto value = mapJson[key].asString();
                typeMap->insert(make_pair(key, value));
            }

            return typeMap;
        }

        inline shared_ptr<unordered_map<string, string>> parseMapJson(const Json::Value &j, const string &name) {
            auto mapJson = j[name];
            return parseMapJson(mapJson);
        }

        inline shared_ptr<vector<string>> parseArrayJsonRaw(const Json::Value &j, const string& name) {
            auto arrayJson = j[name];
            shared_ptr<vector<string>> typeArray = make_shared<vector<string>>();

            if (!arrayJson.isArray()) {
                return typeArray;
            }

            for (const auto &itemJson : arrayJson) {
                typeArray->push_back(itemJson.asString());
            }

            return typeArray;
        }

        // WRITE

        template<typename T>
        inline void writeJson(Json::Value& j, T& value) {
            value.toJson(j);
        }

        template<typename T>
        inline void writeJson(Json::Value& j, shared_ptr<T>& value) {
            if (!value) {
                return;
            }

            writeJson(j, *value);
        }

        template<typename T>
        inline void writeJson(Json::Value& j, const string& name, shared_ptr<T>& value) {
            writeJson(j[name], value); 
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<string>& value) {
            if (!value) {
                return;
            }

            j = *value;
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<int32>& value) {
            if (!value) {
                return;
            }

            j = *value;
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<int64>& value) {
            if (!value) {
                return;
            }

            j = *value;
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<bool>& value) {
            if (!value) {
                return;
            }

            j = *value;
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<Date>& value) {
            if (!value) {
                return;
            }

            value->toJson(j);
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<Datetime>& value) {
            if (!value) {
                return;
            }

            value->toJson(j);
        }

        template<>
        inline void writeJson(Json::Value& j, shared_ptr<Json::Value>& value) {
            if (!value) {
                return;
            }

            j = *value;
        }

        template<typename T>
        inline void writeArrayJson(Json::Value &j,  shared_ptr<vector<T>> valueArray) {
            Json::Value arrayJson(Json::arrayValue);
            for (int i = 0; i < valueArray->size(); i++) {
                writeJson(arrayJson[i], valueArray->at(i));
            }
            
            j = arrayJson;
        }

        template<typename T>
        inline void writeArrayJson(Json::Value &j, const string &name, shared_ptr<vector<T>> valueArray) {
            writeArrayJson(j[name], valueArray);
        }

        inline void writeMapJson(Json::Value &j, const string &name, const shared_ptr<unordered_map<string, string>> &valueMap) {
            Json::Value mapJson;
            for (auto& value : *valueMap) {
                mapJson[value.first] = value.second;
            }

            j[name] = mapJson;
        }
    }
}