#pragma once

#define UI UI_ST
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#undef UI

#include "DecodedJWT.h"
#include <Anticheat.hpp>

namespace minecraft {
	namespace jwt {

		const int VERSION = 96;

#if OPENSSL_VERSION_NUMBER < 269484032
#define OPENSSL10
#endif

		struct rsa_exception : public std::runtime_error {
			explicit rsa_exception(const std::string &msg)
				: std::runtime_error(msg) {}
		};

		struct signature_verification_exception : public std::runtime_error {
			explicit signature_verification_exception(const std::string &msg)
				: std::runtime_error(msg) {}
		};

		class RSA256 {
		public:
			explicit RSA256(const string& pemPublicKey) {
				publicKey = loadPublicKey(pemPublicKey);
			}

			ANTICHEAT_NO_OPTIMIZATION_BEGIN
				int verifyJwt(const std::string& data, const std::string& signature) const {
				ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
#ifdef OPENSSL10
				std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_destroy)> ctx(EVP_MD_CTX_create(), EVP_MD_CTX_destroy);
#else
				std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_create(), EVP_MD_CTX_free);
#endif            	
				if (!ctx)
					throw signature_verification_exception("failed to verify signature: could not create context");
				if (!EVP_VerifyInit(ctx.get(), EVP_sha256()))
					throw signature_verification_exception("failed to verify signature: VerifyInit failed");
				if (!EVP_VerifyUpdate(ctx.get(), data.data(), data.size()))
					throw signature_verification_exception("failed to verify signature: VerifyUpdate failed");
				auto res = EVP_VerifyFinal(ctx.get(), (const unsigned char*)signature.data(), signature.size(), publicKey.get());
				if (res != 1)
					throw signature_verification_exception("evp verify final failed: " + std::to_string(res) + " " + ERR_error_string(ERR_get_error(), nullptr));

				int magicNumber = VERSION;
				ANTICHEAT_VIRT_PROTECT_STRINGS_END;
				return magicNumber;
			}
			ANTICHEAT_NO_OPTIMIZATION_END

		private:
			static shared_ptr<EVP_PKEY> loadPublicKey(const string &key, const string &password = "") {
				unique_ptr<BIO, decltype(&BIO_free_all)> pubkey_bio(BIO_new(BIO_s_mem()), BIO_free_all);

				if ((size_t)BIO_write(pubkey_bio.get(), key.data(), key.size()) != key.size())
					throw rsa_exception("failed to load public key: bio_write failed");

				shared_ptr<EVP_PKEY> pkey(
					PEM_read_bio_PUBKEY(
						pubkey_bio.get(),
						nullptr,
						nullptr,
						(void *)password.c_str()),
					EVP_PKEY_free);

				if (!pkey) {
					auto errorMessage = string(ERR_error_string(ERR_get_error(), nullptr));
					throw rsa_exception("failed to load public key: PEM_read_bio_PUBKEY failed:" + errorMessage);
				}

				return pkey;
			}

		private:
			std::shared_ptr<EVP_PKEY> publicKey;
		};


		class JWTVerifier {
		public:
			explicit JWTVerifier(const string& publicKey)
				: rsa256(publicKey) {
			}

			ANTICHEAT_NO_OPTIMIZATION_BEGIN
				int verifyJWT(const DecodedJWT& jwt) const {
				ANTICHEAT_VIRT_BEGIN;
				const std::string data = jwt.getHeaderBase64() + "." + jwt.getPayloadBase64();
				const std::string& sig = jwt.getSignature();
				auto result = rsa256.verifyJwt(data, sig);
				ANTICHEAT_VIRT_END;
				return result;
			}
			ANTICHEAT_NO_OPTIMIZATION_END

		private:
			RSA256 rsa256;
		};
	}
}