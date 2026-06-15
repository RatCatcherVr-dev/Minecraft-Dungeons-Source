#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"

namespace minecraft {
    namespace api {
        struct Event {
            shared_ptr<string> source;
            shared_ptr<string> name;
            shared_ptr<Datetime> timestamp;
            shared_ptr<Json::Value> data;

            Event() = default;

            template<typename T>
            static Event create(const string& source, const string& name, T data) {
                auto jsonValue = make_shared<Json::Value>();
                data.toJson(*jsonValue);
                return Event(source, name, jsonValue);
            }

            static Event create(const string& source, const string& name) {
                return Event(source, name);
            }

            Event(const string& source, const string& name, shared_ptr<Json::Value> data = nullptr) {
                this->source = make_shared<string>(source);
                this->name = make_shared<string>(name);
                this->timestamp = make_shared<Datetime>();
                this->data = std::move(data);
            }

            void toJson(Json::Value &j) {
                writeJson(j, "source", this->source);
                writeJson(j, "name", this->name);
                writeJson(j, "timestamp", this->timestamp);
                writeJson(j, "data", this->data);
            }

            void fromJson(const Json::Value& j) {
                this->source = parseJson<string>(j, "source");
                this->name = parseJson<string>(j, "name");
                this->timestamp = parseJson<Datetime>(j, "timestamp");
                this->data = parseJson<Json::Value>(j, "data");
            }
        };
    }
}