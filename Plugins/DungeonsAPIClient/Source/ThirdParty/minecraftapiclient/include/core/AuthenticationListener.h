#pragma once

#include "Types.h"
#include "DungeonsLoginResponse.h"

namespace minecraft {
    namespace api {
        class AuthenticationListener {
        public:
            virtual void loginSuccessful(const string& accessToken, const string& userId, const shared_ptr<DungeonsData>& dungeonsData) {}
            virtual void loginFailed() {}
        };
    }
}