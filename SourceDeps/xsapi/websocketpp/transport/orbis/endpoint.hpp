#pragma once

#include <websocketpp/common/memory.hpp>
#include <websocketpp/logger/levels.hpp>

#include <websocketpp/transport/base/endpoint.hpp>
#include "websocketpp/transport/orbis/connection.hpp"
#include "websocketpp/epoll_service.hpp"

namespace websocketpp {
namespace transport {
namespace orbis {

template <typename config>
class endpoint {
public:
	/// Type of this endpoint transport component
	typedef endpoint type;
	/// Type of a pointer to this endpoint transport component
	typedef lib::shared_ptr<type> ptr;

	/// Type of this endpoint's concurrency policy
	typedef typename config::concurrency_type concurrency_type;
	/// Type of this endpoint's error logging policy
	typedef typename config::elog_type elog_type;
	/// Type of this endpoint's access logging policy
	typedef typename config::alog_type alog_type;

	/// Type of this endpoint transport component's associated connection
	/// transport component.
	typedef orbis::connection<config> transport_con_type;
	/// Type of a shared pointer to this endpoint transport component's
	/// associated connection transport component
	typedef typename transport_con_type::ptr transport_con_ptr;

	explicit endpoint()
	{
	}

	/// Set whether or not endpoint can create secure connections
	/**
	 * TODO: docs
	 *
	 * Setting this value only indicates whether or not the endpoint is capable
	 * of producing and managing secure connections. Connections produced by
	 * this endpoint must also be individually flagged as secure if they are.
	 *
	 * @since 0.3.0-alpha4
	 *
	 * @param value Whether or not the endpoint can create secure connections.
	 */
	void set_secure(bool value) {}

	/// Tests whether or not the underlying transport is secure
	/**
	 * TODO: docs
	 *
	 * @return Whether or not the underlying transport is secure
	 */
	bool is_secure() const {
		return true;
	}

	void init(SceNetId resolverId, int sslCtxId) {
		m_epoll_service = std::make_shared<websocketpp::orbis::epoll_service>();
		m_epoll_service->registerDescriptor(resolverId);
		mResolver = resolverId;
		mSslCtxId = sslCtxId;
	}

	void terminate() {
		m_epoll_service->deregisterDescriptor(mResolver);
	}

	void work(int timeoutMilliseconds) {
		m_epoll_service->work(timeoutMilliseconds);
	}

protected:
	/// Initialize logging
	/**
	 * The loggers are located in the main endpoint class. As such, the
	 * transport doesn't have direct access to them. This method is called
	 * by the endpoint constructor to allow shared logging from the transport
	 * component. These are raw pointers to member variables of the endpoint.
	 * In particular, they cannot be used in the transport constructor as they
	 * haven't been constructed yet, and cannot be used in the transport
	 * destructor as they will have been destroyed by then.
	 *
	 * @param a A pointer to the access logger to use.
	 * @param e A pointer to the error logger to use.
	 */
	void init_logging(const lib::shared_ptr<alog_type>& a, const lib::shared_ptr<elog_type>& e) {
		m_alog = a;
		m_elog = e;
	}

	/// Initiate a new connection
	/**
	 * @param tcon A pointer to the transport connection component of the
	 * connection to connect.
	 * @param u A URI pointer to the URI to connect to.
	 * @param cb The function to call back with the results when complete.
	 */
	void async_connect(transport_con_ptr tcon, uri_ptr u, connect_handler cb) {
		tcon->set_uri(u);

		std::shared_ptr<SceNetSockaddrIn> sin = std::make_shared<SceNetSockaddrIn>();

		int sceResult = sceNetResolverStartNtoa(mResolver, u->get_host().c_str(), &sin->sin_addr, 0, 0, SCE_NET_RESOLVER_ASYNC);
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetResolverStartNtoa() failed", sceResult);
			cb(make_error_code(error::resolver));
			return;
		}

		sin->sin_len = sizeof(SceNetSockaddrIn);
		sin->sin_family = SCE_NET_AF_INET;
		sin->sin_port = sceNetHtons(u->get_port());

		if (!m_epoll_service->addCallback(websocketpp::orbis::epoll_service::event_type::pollin, mResolver,
			lib::bind(&type::handle_resolve, this, tcon, cb, std::move(sin))))
		{
			m_elog->write(log::elevel::rerror, "epoll_service::addCallback() failed");
			cb(make_error_code(error::resolver));
		}
	}

	void handle_resolve(transport_con_ptr tcon, connect_handler cb, std::shared_ptr<SceNetSockaddrIn> sin) {
		int sceResult = sceNetConnect(tcon->get_socket(), (SceNetSockaddr*)sin.get(), sizeof(SceNetSockaddr));
		if (sceResult == SCE_NET_ERROR_EINPROGRESS) {
			if (!m_epoll_service->addCallback(websocketpp::orbis::epoll_service::event_type::pollout, tcon->get_socket(),
				lib::bind(&type::handle_connect, this, tcon, cb)))
			{
				m_elog->write(log::elevel::rerror, "epoll_service::addCallback() failed");
				cb(make_error_code(error::connect));
				return;
			}
		}
		else if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetConnect() failed", sceResult);
			cb(make_error_code(error::connect));
		}
	}

	void handle_connect(transport_con_ptr tcon, connect_handler cb) {
		lib::error_code ec;
		int sockopt = 0;
		SceNetSocklen_t sockoptLen = 0;
		int sceResult = sceNetGetsockopt(tcon->get_socket(), SCE_NET_SOL_SOCKET, SCE_NET_SO_ERROR, &sockopt, &sockoptLen);
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetGetsockopt(SCE_NET_SO_ERROR) failed", sceResult);
			cb(make_error_code(error::connect));
			return;
		}
		else if (sockopt < 0) {
			log_sce_err(log::elevel::rerror, "sceNetConnect() failed", sockopt);
			cb(make_error_code(error::connect));
			return;
		}

		sceResult = sceSslCreateConnection(mSslCtxId, tcon->get_socket(), tcon->get_host().c_str());
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceSslCreateConnection() failed", sceResult);
			cb(make_error_code(error::ssl_connect));
			return;
		}
		tcon->set_ssl_connection(sceResult);

		try_ssl_connect(tcon, cb);
	}

	void try_ssl_connect(transport_con_ptr tcon, connect_handler cb) {
		int sceResult = sceSslConnect(tcon->get_ssl_connection());
		if (sceResult == SCE_SSL_ERROR_WANT_POLLIN || sceResult == SCE_SSL_ERROR_WANT_POLLOUT) {
			auto eventType = (sceResult == SCE_SSL_ERROR_WANT_POLLIN
				? websocketpp::orbis::epoll_service::event_type::pollin
				: websocketpp::orbis::epoll_service::event_type::pollout);
			if (!m_epoll_service->addCallback(eventType, tcon->get_socket(),
				lib::bind(&type::try_ssl_connect, this, tcon, cb)))
			{
				m_elog->write(log::elevel::rerror, "epoll_service::addCallback() failed");
				cb(make_error_code(error::ssl_connect));
				return;
			}
		}
		else if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceSslConnect() failed", sceResult);
			cb(make_error_code(error::ssl_connect));
			return;
		}
		else {
			cb(lib::error_code());
		}
	}

	/// Initialize a connection
	/**
	 * Init is called by an endpoint once for each newly created connection.
	 * It's purpose is to give the transport policy the chance to perform any
	 * transport specific initialization that couldn't be done via the default
	 * constructor.
	 *
	 * @param tcon A pointer to the transport portion of the connection.
	 * @return A status code indicating the success or failure of the operation
	 */
	lib::error_code init(transport_con_ptr tcon) {
		return tcon->init_socket(m_epoll_service);
	}

private:
	/// Convenience method for logging the code and message for an error_code
	void log_sce_err(log::level l, const char * msg, int error) {
		std::stringstream s;
		s << msg << " sce_error: 0x" << std::setfill('0') << std::setw(8) << std::hex << error;
		m_elog->write(l, s.str());
	}

	lib::shared_ptr<elog_type> m_elog;
	lib::shared_ptr<alog_type> m_alog;

	lib::shared_ptr<websocketpp::orbis::epoll_service> m_epoll_service;

	SceNetId mResolver{-1};
	int mSslCtxId{-1};
};

} // namespace orbis
} // namespace transport
} // namespace websocketpp
