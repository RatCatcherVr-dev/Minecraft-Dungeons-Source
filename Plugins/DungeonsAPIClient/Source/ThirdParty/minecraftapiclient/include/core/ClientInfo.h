#pragma once

#include <utility>

#include "Types.h"

namespace minecraft {
    namespace api {
        class ClientInfo {
        public:
            ClientInfo(string clientName, string  clientVersion, string  clientBuild)
                : clientName(std::move(clientName)),
                clientVersion(std::move(clientVersion)),
                clientBuild(std::move(clientBuild)) {}

            const string &getClientName() const {
                return clientName;
            }

            const string &getClientVersion() const {
                return clientVersion;
            }

            const string &getClientBuild() const {
                return clientBuild;
            }

            static string getSDKVersion() {
                return "0.7.0";
            }

            string getClientVersionFull() const {
                return getClientVersion() + "/" + getSDKVersion();
            }

        private:
            const string clientName;
            const string clientVersion;
            const string clientBuild;
        };
    }
}