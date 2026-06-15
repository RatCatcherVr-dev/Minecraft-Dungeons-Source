#pragma once

#include <core/Authenticator.h>
#include <core/Config.h>
#include <core/Types.h>

#include "ValidatedEntitlement.h"
#include "EntitlementsResponse.h"
#include "Entitlement.h"

#include <algorithm>
#include <cctype>
#include <jwt/JWTVerifier.h>
#include <unordered_set>

#include <Anticheat.hpp>

namespace minecraft {
    namespace api {
        class JWTValidationException : public std::exception {};

        class EntitlementsValidator {
        public:
            explicit EntitlementsValidator(const Config& config) {
                verifier = make_shared<jwt::JWTVerifier>(config.getPublicKey());
            }

        	ANTICHEAT_NO_OPTIMIZATION_BEGIN
            // Validates the response, gives back all entitlements that are listed in the outer jwt.
            vector<ValidatedEntitlement> validate(
                    const EntitlementsResponse& response,
                    const string& signerId,
                    const string& challengeResponse) {
                try {
                	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
                    auto &signature = *response.signature;
                    auto decoded = jwt::DecodedJWT::decode(signature);

                    auto signerIdClaim = decoded.getPayloadClaim("signerId").asString();
                    if (signerIdClaim != signerId) {
                        throw JWTValidationException();
                    }

                    auto requestIdChallengeResponseClaim = decoded.getPayloadClaim("ridcr").asString();
                    if (requestIdChallengeResponseClaim != challengeResponse) {
                        throw JWTValidationException();
                    }

					auto result = verifier->verifyJWT(decoded);
                	if (result != minecraft::jwt::VERSION) {
                		throw JWTValidationException();
                	}

                    std::string responsePlatform = toLower(decoded.getPayloadClaim("platform").asString());
                    std::string ourPlatform = toLower(GetServicesPlatformType());
                    if (ourPlatform != responsePlatform) {
                        throw JWTValidationException();
                    }

                    std::unordered_set<string> validNames;
                    auto entitlements = decoded.getPayloadClaim("entitlements").asArray();
                    for (auto entitlement : entitlements) {
                        auto entitlementName = entitlement["name"].asString();
                        validNames.insert(entitlementName);
                    }

                    vector<ValidatedEntitlement> validatedEntitlements;
                    for (const auto& item : *response.items) {
                        try {
                            auto validEntitlement = validateSignature(*item.signature, signerId, false);
                            if (validNames.find(validEntitlement.getEntitlementName()) != validNames.end()) {
                                validatedEntitlements.push_back(validEntitlement);
                            }
                        } catch (std::exception&) {
                            // ignore
                        }
                    }

                	ANTICHEAT_VIRT_PROTECT_STRINGS_END;
                    return validatedEntitlements;
                } catch(std::exception&) {
                    return {};
                }
            }
        	ANTICHEAT_NO_OPTIMIZATION_END

            // Validates one entitlement.
            // Returns a vector with one element or a empty vector if the entitlement could not be verified.
            vector<ValidatedEntitlement> validate(const Entitlement& entitlement, const string& signerId) {
                return validate(*entitlement.signature, signerId);
            }

            // Validates one entitlement signature.
            // Returns a vector with one element or a empty vector if the entitlement could not be verified.
            vector<ValidatedEntitlement> validate(
                    const string& signature,
                    const string& signerId,
                    bool verifySignature = true) {
                try {
                    auto validatedEntitlement = validateSignature(signature, signerId, verifySignature);
                    return {validatedEntitlement};
                } catch(std::exception&) {
                    return {};
                }
            }

        private:
			ANTICHEAT_NO_OPTIMIZATION_BEGIN
            //Validates the signature of one Entitlement
            // NOTE: this can throw an exception, anyone calling it should use a try - catch
            ValidatedEntitlement validateSignature(
                    const string& signature,
                    const string& signerId,
                    bool verifySignature = true) {
            	ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
                auto decoded = jwt::DecodedJWT::decode(signature);

                auto singerIdClaim = decoded.getPayloadClaim("signerId").asString();
                auto entitlementNameClaim = decoded.getPayloadClaim("name").asString();

                if (singerIdClaim != signerId) {
                    throw JWTValidationException();
                }

                if (verifySignature) {                	
                    auto result = verifier->verifyJWT(decoded);
                	if (result != minecraft::jwt::VERSION) {
                		throw JWTValidationException();
                	}
                }
            	ANTICHEAT_VIRT_PROTECT_STRINGS_END;

                return {entitlementNameClaim, singerIdClaim, signature};
            }

        	ANTICHEAT_NO_OPTIMIZATION_END

            static std::string toLower(const std::string& str) {
                std::string result;
                result.reserve(str.size());
                std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
                return result;
            }

        private:
            shared_ptr<jwt::JWTVerifier> verifier;
        };
    }
}