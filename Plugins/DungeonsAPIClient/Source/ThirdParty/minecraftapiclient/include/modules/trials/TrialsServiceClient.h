#pragma once

#include "core/HttpServiceClient.h"
#include "CompletedTrialRequest.h"
#include "TrialsApiResponse.h"
#include <string>
#include <Anticheat.hpp>

namespace minecraft {
	namespace api {
		class TrialsServiceClient {
		public:
			explicit TrialsServiceClient(shared_ptr<HttpServiceClient> client, shared_ptr<HttpHeaders> headers)
				: client(move(client)), headers(move(headers)) {
			}

			void completeTrial(const CompletedTrialRequest &body, const function<void(HttpServiceResponse<string>)> &callback);
			void getTrialsByOffset(const string &game, const string &offset, const function<void(HttpServiceResponse<TrialsApiResponse>)> &callback);
			void getTrialsByOffsetAnonymous(const string& game, const string& offset, const function<void(HttpServiceResponse<TrialsApiResponse>)> &callback);

		private:
			shared_ptr<HttpServiceClient> client;
			shared_ptr<HttpHeaders> headers;
		};

		ANTICHEAT_NO_OPTIMIZATION_BEGIN;
		inline void TrialsServiceClient::completeTrial(const CompletedTrialRequest &body, const function<void(HttpServiceResponse<string>)> &callback) {
			headers->fetchHeaders([=](const auto &requestHeaders) {
				ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
				string path("/trials/complete/");
				return client->request<string>("POST", path, requestHeaders, body, callback);
				ANTICHEAT_VIRT_PROTECT_STRINGS_END;
			});
		}

		inline void TrialsServiceClient::getTrialsByOffset(const string &game, const string &offset, const function<void(HttpServiceResponse<TrialsApiResponse>)> &callback) {
			headers->fetchHeaders([=](const auto &requestHeaders) {
				ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
				string path("/trials/game/{game}/offset/{offset}/");
				replace(path, "game", game);
				replace(path, "offset", offset);
				return client->request<TrialsApiResponse>("GET", path, requestHeaders, callback);
				ANTICHEAT_VIRT_PROTECT_STRINGS_END;
			});
		}

		inline void TrialsServiceClient::getTrialsByOffsetAnonymous(const string& game, const string& offset, const function<void(HttpServiceResponse<TrialsApiResponse>)> &callback) {
			headers->fetchHeaders([=](auto &requestHeaders) {
				ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN;
				requestHeaders.push_back({ "X-Trials-Key", "qMJ6bj3P9qlpQ7CMlk5LqaB5fqF0nHOQ5ffvQcSd0ApmDUjypwgZG9XxGxQr6E0i" });

				string path("/trials/open/game/{game}/offset/{offset}/");
				replace(path, "game", game);
				replace(path, "offset", offset);
				return client->request<TrialsApiResponse>("GET", path, requestHeaders, callback);
				ANTICHEAT_VIRT_PROTECT_STRINGS_END;
			});
		}
		ANTICHEAT_NO_OPTIMIZATION_END;
	}
}
