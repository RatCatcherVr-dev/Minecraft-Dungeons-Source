#pragma once

#include <string>
#include <vector>
#include <future>
#include <functional>

using std::string;
using std::vector;
using std::future;
using std::function;

namespace minecraft {
    namespace api {
        struct HttpHeader {
            string key;
            string value;
        };

        typedef string http_method;

        struct HttpRequest {
            vector<HttpHeader> headers;
            http_method method;
            string domain;
            string path;
            string body;
        };

        struct HttpResponse {
            HttpResponse() : status(503), body("") {}
            explicit HttpResponse(const uint16_t status) : status(status), body("") {}
            HttpResponse(const uint16_t status, string body, vector<HttpHeader> headers = {})
                : status(status), headers(std::move(headers)), body(std::move(body)) {}

            uint16_t status;
            vector<HttpHeader> headers;
            string body;
        };


        class HttpClient {
        public:
            virtual void request(const HttpRequest &httpRequest, const function<void(HttpResponse)> &response) = 0;
        };
    }
}