#pragma once

#include "Types.h"
#include "HttpClient.h"

namespace minecraft {
    namespace api {
        template<typename T>
        class HttpServiceResponse {
        public:
            explicit HttpServiceResponse(uint16_t status)
                : status(status) {}
            HttpServiceResponse(uint16_t status, vector<shared_ptr<T>> body) 
                : status(status), body(move(body)) {}

            uint16_t getStatus() { return status; }
            shared_ptr<T> getBody() { return body.empty() ? nullptr : body.front(); }
            const vector<shared_ptr<T>>& getBodyList() { return body; }

            bool successful() { return status >= 200 && status <= 399; }
            bool failed() { return !successful(); }

        private: 
            uint16_t status;
            vector<shared_ptr<T>> body;
        };
    }
}