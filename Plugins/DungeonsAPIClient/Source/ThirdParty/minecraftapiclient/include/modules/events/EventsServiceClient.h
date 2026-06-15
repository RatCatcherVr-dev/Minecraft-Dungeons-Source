#pragma once

#include "core/HttpServiceClient.h"
#include "EventsRequest.h"
#include "EventResponse.h"
#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        class EventsServiceClient
        {
        public:
            explicit EventsServiceClient(shared_ptr<HttpServiceClient> client, shared_ptr<HttpHeaders> headers)
                    : client(move(client)), headers(move(headers)) {}

            void pushEvents(const EventsRequest &request, const function<void(HttpServiceResponse<EventsResponse>)> &callback);

        private:
            shared_ptr<HttpServiceClient> client;
            shared_ptr<HttpHeaders> headers;
        };

    	ANTICHEAT_NO_OPTIMIZATION_BEGIN;
        inline void EventsServiceClient::pushEvents(const EventsRequest &request, const function<void(HttpServiceResponse<EventsResponse>)> &callback) {
            headers->fetchHeaders([=](auto &requestHeaders) {
            	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
                string path("/events/");
                return client->request<EventsResponse>("POST", path, requestHeaders, request, callback);
            	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
            });
        }
    	ANTICHEAT_NO_OPTIMIZATION_END;
    }
}