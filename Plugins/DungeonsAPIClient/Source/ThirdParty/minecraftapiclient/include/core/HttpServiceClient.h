#pragma once

#include <exception>
#include "HttpServiceResponse.h"
#include "JsonParser.h"
#include "JsonSerializer.h"
#include "TimerManager.h"

using std::exception;

namespace minecraft {
    namespace api {
        class HttpServiceClient {
        public:
            explicit HttpServiceClient(string domain, shared_ptr<HttpClient> client)
                : domain(move(domain)), client(move(client)) {}

            template<typename T>
            void request(const http_method &method, const string &path, const vector<HttpHeader>& headers, function<void(HttpServiceResponse<T>)> callback);

            template<typename T, typename U>
            void request(const http_method &method, const string &path, const vector<HttpHeader>& headers, U data, function<void(HttpServiceResponse<T>)> callback);

            template<typename T, typename U>
            void request(const http_method &method, const string &path, const vector<HttpHeader>& headers, vector<U> data, function<void(HttpServiceResponse<T>)> callback);

            FTimerManager& getTimerManager();

        private:
            void makeRequest(const http_method &method, const string &path, const vector<HttpHeader>& headers, string &data, const function<void(HttpResponse)> &callback);

        private:
            template<typename T>
            HttpServiceResponse<T> deserialize(const HttpResponse &response);

            template<typename T>
            string serialize(vector<T> &type);

        private:
            const string domain;
            shared_ptr<HttpClient> client;
            JsonParser parser;
        };

        template<typename T>
        inline void HttpServiceClient::request(const http_method &method, const string &path, const vector<HttpHeader>& headers, function<void(HttpServiceResponse<T>)> callback) {
            vector<string> emptyPostBody = {""};
            request<T, string>(method, path, headers, emptyPostBody, callback);
        }

        template<typename T, typename U>
        inline void HttpServiceClient::request(const http_method &method, const string &path, const vector<HttpHeader>& headers, U data, function<void(HttpServiceResponse<T>)> callback) {
            vector<U> bodyData = {data};
            request<T,U>(method, path, headers, bodyData, callback);
        }

        template<typename T, typename U>
        inline void HttpServiceClient::request(const http_method &method, const string &path, const vector<HttpHeader>& headers, vector<U> data, function<void(HttpServiceResponse<T>)> callback) {
            auto serializedData = serialize(data);
            makeRequest(method, path, headers, serializedData, [=](HttpResponse response){
                try {
                    auto serviceResponse = deserialize<T>(response);
                    callback(serviceResponse);
                } catch(std::exception&) {
                    callback(HttpServiceResponse<T>(503));
                }
            });
        }

        inline void HttpServiceClient::makeRequest(const http_method &method, const string &path, const vector<HttpHeader>& headers, string &data, const function<void(HttpResponse)> &callback) {
            HttpRequest request;
            request.domain = this->domain;
            request.path =  path;
            request.method = method;
            request.headers = headers;

            if (!data.empty()) {
                request.body = data;
            }
            
            client->request(request, callback);
        }

        template<>
        inline HttpServiceResponse<string> HttpServiceClient::deserialize(const HttpResponse &response) {
            return HttpServiceResponse<string>(response.status, {make_shared<string>(response.body)});
        }

        template<typename T>
        inline HttpServiceResponse<T> HttpServiceClient::deserialize(const HttpResponse &response) {
            vector<shared_ptr<T>> itemList;

            auto parseResult = parser.parse(response.body);
            if (parseResult.parsed) {
                auto responseJson = parseResult.jsonValue;
                if (responseJson.isArray()) {
                    for (const auto &itemJson : responseJson) {
                        auto item = parseJson<T>(itemJson);
                        itemList.push_back(item);
                    }
                } else {
                    auto item = parseJson<T>(responseJson);
                    itemList.push_back(item);
                }
            }

            return HttpServiceResponse<T>(response.status, itemList);
        }

        template<typename T>
        inline string HttpServiceClient::serialize(vector<T> &types) {
            if (types.size() == 1) {
                Json::Value jsonResult;
                writeJson(jsonResult, types.front());
                return parser.write(jsonResult);
            } 
                
            Json::Value jsonResult(Json::arrayValue);
            for (auto type : types) {
                Json::Value j;
                writeJson(j, type);
                jsonResult.append(j);
            }

            return parser.write(jsonResult);
        }

        template<>
        inline string HttpServiceClient::serialize(vector<string> &types) {
            return "";
        }
    }
}