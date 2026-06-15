#pragma once

#include "HttpServiceClient.h"
#include "LoginRequest.h"
#include "LoginResponse.h"
#include "DungeonsLoginResponse.h"
#include <Anticheat.hpp>

namespace minecraft {
namespace api {
    class AuthenticationServiceClient
    {
    public:
        explicit AuthenticationServiceClient(shared_ptr<HttpServiceClient> client)
            : client(move(client)) {}

    	void dungeonsLogin(const LoginRequest &request, const function<void(HttpServiceResponse<DungeonsLoginResponse>)> &callback);

    private:
        shared_ptr<HttpServiceClient> client;
    };

	ANTICHEAT_NO_OPTIMIZATION_BEGIN

	inline void AuthenticationServiceClient::dungeonsLogin(const LoginRequest &request, const function<void(HttpServiceResponse<DungeonsLoginResponse>)> &callback) {
    	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
        string path("/dungeons/login/");
        vector<HttpHeader> requestHeaders = {{"Content-Type", "application/json"}};
        return client->request<DungeonsLoginResponse>("POST", path, requestHeaders, request, callback);
    	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
    }
	ANTICHEAT_NO_OPTIMIZATION_END
}
}
