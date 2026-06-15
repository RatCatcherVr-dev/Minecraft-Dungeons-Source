#pragma once

#include "core/HttpServiceClient.h"

namespace minecraft {
	namespace api {
		class LiveOpsServiceClient {
		public:
			explicit LiveOpsServiceClient(shared_ptr<HttpServiceClient> inClient, shared_ptr<HttpHeaders> inHeaders) 
				: client(std::move(inClient)), headers(std::move(inHeaders)) {}

			template <endpoint::Dungeons Endpoint, typename Callback, typename Post>
			void request(const Callback&, const Post&);
		private:
			shared_ptr<HttpServiceClient> client;
			shared_ptr<HttpHeaders> headers;
		};

		template <endpoint::Dungeons Endpoint, typename Callback, typename Post>
		inline void LiveOpsServiceClient::request(const Callback& callback, const Post& post) {
			headers->fetchHeaders([=](auto &requestHeaders) {
				auto endpoint = endpoint::get<Endpoint>();
				return client->request(endpoint.verb, endpoint.path, requestHeaders, post, callback);
			});
		}
	}
}

