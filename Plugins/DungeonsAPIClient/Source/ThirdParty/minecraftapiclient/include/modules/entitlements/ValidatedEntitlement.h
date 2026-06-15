#pragma once

#include <core/Types.h>

namespace minecraft {
    namespace api {
        class ValidatedEntitlement {
        public:
            ValidatedEntitlement(string  entitlementName, string  signerId, string signature)
                : entitlementName(std::move(entitlementName)),
                signerId(std::move(signerId)),
                signature(move(signature)) {}

            const string& getEntitlementName() const {
                return entitlementName;
            }

            const string& getSignerId() const {
                return signerId;
            }

            const string& getSignature() const {
                return signature;
            }

        private:
            string entitlementName;
            string signerId;
            string signature;
        };
    }
}