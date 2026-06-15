#pragma once

#include "core/Types.h"
#include "core/JsonSerializer.h"
#include "Event.h"

namespace minecraft {
    namespace api {
        struct EventsRequest {
            shared_ptr<vector<Event>> events;

            EventsRequest() {
                this->events = make_shared<vector<Event>>();
            }

            explicit EventsRequest(const vector<Event>& events) {
                this->events = make_shared<vector<Event>>(events);
            }

            void add(const Event& event) {
                events->push_back(event);
            }

            void toJson(Json::Value &j) {
                writeArrayJson(j, "events", this->events);
            }

            void fromJson(const Json::Value& j) {
                this->events = parseArrayJson<Event>(j, "events");
            }
        };
    }
}