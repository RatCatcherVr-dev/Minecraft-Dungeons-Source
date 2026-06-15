#pragma once

#include <websocketpp/config/core_client.hpp>

#include "websocketpp/transport/orbis/endpoint.hpp"

namespace websocketpp {
namespace config {

/// Client config with custom transport layer using Orbis SSL library
struct orbis_client : public core_client {
	typedef orbis_client type;
	typedef core_client base;

	typedef base::concurrency_type concurrency_type;

	typedef base::request_type request_type;
	typedef base::response_type response_type;

	typedef base::message_type message_type;
	typedef base::con_msg_manager_type con_msg_manager_type;
	typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

	typedef base::alog_type alog_type;
	typedef base::elog_type elog_type;

	typedef base::rng_type rng_type;

	struct transport_config : public base::transport_config {
		typedef type::concurrency_type concurrency_type;
		typedef type::alog_type alog_type;
		typedef type::elog_type elog_type;
		typedef type::request_type request_type;
		typedef type::response_type response_type;
	};

	typedef websocketpp::transport::orbis::endpoint<transport_config>
		transport_type;

	// Uncomment for log spam
	//static const websocketpp::log::level elog_level =
	//	websocketpp::log::elevel::all ^ websocketpp::log::elevel::devel;
	//static const websocketpp::log::level alog_level =
	//	websocketpp::log::alevel::all;
};

} // namespace config
} // namespace websocketpp
