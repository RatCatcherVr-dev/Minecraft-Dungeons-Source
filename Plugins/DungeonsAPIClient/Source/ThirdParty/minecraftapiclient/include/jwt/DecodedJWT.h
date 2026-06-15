#pragma once

#include "Claim.h"
#include "Base64.h"

namespace minecraft {
    namespace jwt {
        class DecodedJWT {
        public:
            explicit DecodedJWT(const string& token)
                : token(token) {
                auto hdr_end = token.find('.');
                if (hdr_end == string::npos)
                    throw std::invalid_argument("invalid token supplied");

                auto payload_end = token.find('.', hdr_end + 1);
                if (payload_end == string::npos)
                    throw std::invalid_argument("invalid token supplied");

                header = headerBase64 = token.substr(0, hdr_end);
                payload = payloadBase64 = token.substr(hdr_end + 1, payload_end - hdr_end - 1);
                signature = signatureBase64 = token.substr(payload_end + 1);

                fixPadding(header);
                fixPadding(payload);
                fixPadding(signature);

                header = base::decode<base64url>(header);
                payload = base::decode<base64url>(payload);
                signature = base::decode<base64url>(signature);

                headerClaims = Claim::parseClaims(header);
                payloadClaims = Claim::parseClaims(payload);
            }

            static DecodedJWT decode(const string& token) {
                return DecodedJWT(token);
            }

            const string& getToken() const { return token; }
            const string& getHeader() const { return header; }
            const string& getPayload() const { return payload; }
            const string& getSignature() const { return signature; }
            const string& getHeaderBase64() const { return headerBase64; }
            const string& getPayloadBase64() const { return payloadBase64; }
            const string& getSignatureBase64() const { return signatureBase64; }

            bool hasPayloadClaim(const string& name) {
                return payloadClaims.count(name) != 0;
            }

            const Claim& getPayloadClaim(const string& name) {
                if (!hasPayloadClaim(name))
                    throw std::runtime_error("claim not found");

                return payloadClaims.at(name);
            }

        private:
            static void fixPadding(string& str) {
                switch (str.size() % 4) {
                    case 1:
                        str += base64url::fill();
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(fallthrough)
                        [[fallthrough]];
#endif
#endif
                    case 2:
                        str += base64url::fill();
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(fallthrough)
                        [[fallthrough]];
#endif
#endif
                    case 3:
                        str += base64url::fill();
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(fallthrough)
                        [[fallthrough]];
#endif
#endif
                    default:
                        break;
                }
            }

        private:
            const string token;
            string header;
            string headerBase64;
            string payload;
            string payloadBase64;
            string signature;
            string signatureBase64;

            unordered_map<string, Claim> headerClaims;
            unordered_map<string, Claim> payloadClaims;
        };
    }
}