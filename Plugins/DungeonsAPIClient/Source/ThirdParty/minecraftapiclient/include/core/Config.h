#pragma once

#include "Types.h"
#include <Themida/Anticheat.hpp>

namespace minecraft {
    namespace api {
        class Config {
        public:
            Config(string  endpoint, string  publicKey)
                : endpoint(std::move(endpoint)), publicKey(std::move(publicKey)) {}
			
        	ANTICHEAT_NO_OPTIMIZATION_BEGIN
            static Config createForProd() {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                string prodPubKey = R"(-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAtz7jy4jRH3psj5AbVS6W
NHjniqlr/f5JDly2M8OKGK81nPEq765tJuSILOWrC3KQRvHJIhf84+ekMGH7iGlO
4DPGDVb6hBGoMMBhCq2jkBjuJ7fVi3oOxy5EsA/IQqa69e55ugM+GJKUndLyHeNn
X6RzRzDT4tX/i68WJikwL8rR8Jq49aVJlIEFT6F+1rDQdU2qcpfT04CBYLM5gMxE
fWRl6u1PNQixz8vSOv8pA6hB2DU8Y08VvbK7X2ls+BiS3wqqj3nyVWqoxrwVKiXR
kIqIyIAedYDFSaIq5vbmnVtIonWQPeug4/0spLQoWnTUpXRZe2/+uAKN1RY9mmaB
pRFV/Osz3PDOoICGb5AZ0asLFf/qEvGJ+di6Ltt8/aaoBuVw+7fnTw2BhkhSq1S/
va6LxHZGXE9wsLj4CN8mZXHfwVD9QG0VNQTUgEGZ4ngf7+0u30p7mPt5sYy3H+Fm
sWXqFZn55pecmrgNLqtETPWMNpWc2fJu/qqnxE9o2tBGy/MqJiw3iLYxf7U+4le4
jM49AUKrO16bD1rdFwyVuNaTefObKjEMTX9gyVUF6o7oDEItp5NHxFm3CqnQRmch
HsMs+NxEnN4E9a8PDB23b4yjKOQ9VHDxBxuaZJU60GBCIOF9tslb7OAkheSJx5Xy
EYblHbogFGPRFU++NrSQRX0CAwEAAQ==
-----END PUBLIC KEY-----)";
				string prodEndpoint = "https://api.minecraftservices.com";
				ANTICHEAT_PROTECT_STRINGS_END;
				return Config(prodEndpoint, prodPubKey);
            }
        	ANTICHEAT_NO_OPTIMIZATION_END

        	ANTICHEAT_NO_OPTIMIZATION_BEGIN
            static Config createForStage() {
            	ANTICHEAT_PROTECT_STRINGS_BEGIN;
                string stagePubKey = R"(-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAsKD2mat5kulFYwJwO2gV
1bxW38V06N/3VmHBWD3J/0mHoD1MGwHMMx2X3NpBwu5fJyj7/JhjySCA2mBmz1+l
z6tUc6kEnxoP4HM7UrMUQjfZPm/YzDyXcgLNtAbibrwIAvBZRVLm2zpcjbOvVKm3
o8WHEaMP/sZwRIHo+yM2eciDibk8q0QD8RSSETKth8DU8zYTekAILqypIzqCVCJ8
wegn1O5D8g5ivGycucNZEmbqtk5klXHU4IuzYqbReZznaKc5WdjuDl7yRVKB9snE
u1jyD7swseyjHOSeJJRCmCLZVPMLd6SxlwvWzOHdghpVto6M9xHYjho4ejD3uIDu
uzWS96nidwtJgcy/W4P41vVtZT0niI8gFITx4Bzr3dEhC9KmKu9/77/PxElBvRPd
S+vStXpTt2Pq3tKYEEa+fInNFyrxyUtaBfMWZjNiSUblhZYeu1CW11+UD+9CUC3J
bIgVgjzSbTM7lKE5v7FyJ/rLha266n4wiQQNOxwNTMUKgO5ioBrSPAjqZY7mhRqn
ZtBRNe93Z6kou5lnzFeB7245yVUtlViwTtYv4aw2Z/KZr11EltmHFtvlX2gWFW3/
VgGjLvuThD0Gzdj3WxcWcv0HA0DReICYfMb4naxh01kt99YzkTFcC+U0+/y8D+Or
c0dVeAp+mQajE3UOHc1T97MCAwEAAQ==
-----END PUBLIC KEY-----)";
				string stageEndpoint = "https://api-staging.minecraftservices.com";
				ANTICHEAT_PROTECT_STRINGS_END;
				return Config(stageEndpoint, stagePubKey);
            }
        	ANTICHEAT_NO_OPTIMIZATION_END

            const string& getApiEndpoint() const {
                return endpoint;
            }

            const string& getPublicKey() const {
                return publicKey;
            }

        private:
            string endpoint;
            string publicKey;
        };
    }
}