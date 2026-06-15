#pragma once

#include "base.hpp"

#include <websocketpp/transport/base/connection.hpp>

#include <websocketpp/logger/levels.hpp>

#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/platforms.hpp>

#include "websocketpp/epoll_service.hpp"

#include <string>
#include <vector>

#include <net.h>
#include <libssl.h>

namespace websocketpp {
namespace transport {
namespace orbis {

/// Empty timer class to stub out for timer functionality that orbis
/// transport doesn't support
struct timer {
	void cancel() {}
};

template <typename config>
class connection : public lib::enable_shared_from_this< connection<config> > {
public:
	/// Type of this connection transport component
	typedef connection<config> type;
	/// Type of a shared pointer to this connection transport component
	typedef lib::shared_ptr<type> ptr;

	/// transport concurrency policy
	typedef typename config::concurrency_type concurrency_type;
	/// Type of this transport's access logging policy
	typedef typename config::alog_type alog_type;
	/// Type of this transport's error logging policy
	typedef typename config::elog_type elog_type;

	// Concurrency policy types
	typedef typename concurrency_type::scoped_lock_type scoped_lock_type;
	typedef typename concurrency_type::mutex_type mutex_type;

	typedef lib::shared_ptr<timer> timer_ptr;

    // connection is friends with its associated endpoint to allow the endpoint
    // to call private/protected utility methods that we don't want to expose
    // to the public api.
    friend class endpoint<config>;

	explicit connection(bool is_server, const lib::shared_ptr<alog_type> & alog, const lib::shared_ptr<elog_type> & elog)
	  : m_alog(alog), m_elog(elog)
	{
		m_alog->write(log::alevel::devel,"orbis con transport constructor");
	}

	/// Get a shared pointer to this component
	ptr get_shared() {
		return type::shared_from_this();
	}

	/// Set whether or not this connection is secure
	/**
	 * Todo: docs
	 *
	 * @since 0.3.0-alpha4
	 *
	 * @param value Whether or not this connection is secure.
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

	/// Set uri hook
	/**
	 * Called by the endpoint as a connection is being established to provide
	 * the uri being connected to to the transport layer.
	 *
	 * Implementation is optional and can be ignored if the transport has no
	 * need for this information.
	 *
	 * @since 0.6.0
	 *
	 * @param u The uri to set
	 */
	void set_uri(uri_ptr u) {
		m_alog->write(log::alevel::devel,"orbis connection set_uri");
		m_uri = u;
	}

	/// Set human readable remote endpoint address
	/**
	 * Sets the remote endpoint address returned by `get_remote_endpoint`. This
	 * value should be a human readable string that describes the remote
	 * endpoint. Typically an IP address or hostname, perhaps with a port. But
	 * may be something else depending on the nature of the underlying
	 * transport.
	 *
	 * If none is set a default is returned.
	 *
	 * @since 0.3.0-alpha4
	 *
	 * @param value The remote endpoint address to set.
	 */
	void set_remote_endpoint(std::string value) {}

	/// Get human readable remote endpoint address
	/**
	 * TODO: docs
	 *
	 * This value is used in access and error logs and is available to the end
	 * application for including in user facing interfaces and messages.
	 *
	 * @return A string identifying the address of the remote endpoint
	 */
	std::string get_remote_endpoint() const {
		return "unknown (orbis transport)";
	}

	/// Get the connection handle
	/**
	 * @return The handle for this connection.
	 */
	connection_hdl get_handle() const {
		return m_handle;
	}

	/// Call back a function after a period of time.
	/**
	 * Timers are not implemented in this transport. The timer pointer will
	 * always be empty. The handler will never be called.
	 *
	 * @param duration Length of time to wait in milliseconds
	 * @param callback The function to call back when the timer has expired
	 * @return A handle that can be used to cancel the timer if it is no longer
	 * needed.
	 */
	timer_ptr set_timer(long duration, timer_handler handler) {
		return timer_ptr();
	}

	SceNetId get_socket() const {
		return m_socket;
	}

	const std::string& get_host() const {
		return m_uri->get_host();
	}

	void set_ssl_connection(int id) {
		m_ssl_con = id;
	}

	int get_ssl_connection() const {
		return m_ssl_con;
	}

protected:
	/// Initialize the connection transport
	/**
	 * Initialize the connection's transport component.
	 *
	 * @param handler The `init_handler` to call when initialization is done
	 */
	void init(init_handler handler) {
		m_alog->write(log::alevel::devel,"orbis connection init");
		handler(lib::error_code());
	}

	lib::error_code init_socket(lib::shared_ptr<websocketpp::orbis::epoll_service> epoll_service) {
		m_epoll_service = std::move(epoll_service);
		int sceResult = sceNetSocket("endpoint", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetSocket() failed", sceResult);
			return make_error_code(error::general);
		}
		m_socket = sceResult;

		int sockopt = 1;
		sceResult = sceNetSetsockopt(m_socket, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &sockopt, sizeof(sockopt));
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetSetsockopt(SCE_NET_SO_NBIO) failed", sceResult);
			return make_error_code(error::general);
		}

		sceResult = m_epoll_service->registerDescriptor(m_socket);
		if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceNetEpollControl() failed", sceResult);
			return make_error_code(error::general);
		}
		return lib::error_code();
	}

	/// Initiate an async_read for at least num_bytes bytes into buf
	/**
	 * Initiates an async_read request for at least num_bytes bytes. The input
	 * will be read into buf. A maximum of len bytes will be input. When the
	 * operation is complete, handler will be called with the status and number
	 * of bytes read.
	 *
	 * This method may or may not call handler from within the initial call. The
	 * application should be prepared to accept either.
	 *
	 * The application should never call this method a second time before it has
	 * been called back for the first read. If this is done, the second read
	 * will be called back immediately with a double_read error.
	 *
	 * If num_bytes or len are zero handler will be called back immediately
	 * indicating success.
	 *
	 * @param num_bytes Don't call handler until at least this many bytes have
	 * been read.
	 * @param buf The buffer to read bytes into
	 * @param len The size of buf. At maximum, this many bytes will be read.
	 * @param handler The callback to invoke when the operation is complete or
	 * ends in an error
	 */
	void async_read_at_least(size_t num_bytes, char * buf, size_t len,
		read_handler handler)
	{
		m_alog->write(log::alevel::devel, "orbis_con async_read_at_least");

		try_read(0, num_bytes, buf, len, handler);
	}

	void try_read(size_t bytes_read, size_t num_bytes, char * buf, size_t len,
		read_handler handler) {

		int sceResult = -1;
		do {
			sceResult = sceSslRead(m_ssl_con, buf, len, SCE_SSL_MSG_FLAG_PARTIAL_READ);
			if (sceResult > 0) {
				bytes_read += sceResult;
			}
			else {
				break;
			}
		} while (bytes_read < num_bytes);

		if (sceResult == 0) {
			handler(make_error_code(transport::error::eof), 0);
		}
		else if (sceResult == SCE_SSL_ERROR_WANT_POLLIN || sceResult == SCE_SSL_ERROR_WANT_POLLOUT) {
			auto eventType = (sceResult == SCE_SSL_ERROR_WANT_POLLIN
				? websocketpp::orbis::epoll_service::event_type::pollin
				: websocketpp::orbis::epoll_service::event_type::pollout);
			if (!m_epoll_service->addCallback(eventType, m_socket,
				lib::bind(&type::try_read, get_shared(), bytes_read, num_bytes, buf, len, handler)))
			{
				m_elog->write(log::elevel::rerror, "epoll_service::addCallback() failed");
				handler(make_error_code(error::read), 0);
			}
		}
		else if (sceResult < 0) {
			log_sce_err(log::elevel::rerror, "sceSslRead() failed", sceResult);
			handler(make_error_code(error::read), 0);
		}
		else {
			handler(lib::error_code(), bytes_read);
		}
	}

	/// Asynchronous Transport Write
	/**
	 * Write len bytes in buf to the output stream. Call handler to report
	 * success or failure. handler may or may not be called during async_write,
	 * but it must be safe for this to happen.
	 *
	 * Will return 0 on success.
	 *
	 * @param buf buffer to read bytes from
	 * @param len number of bytes to write
	 * @param handler Callback to invoke with operation status.
	 */
	void async_write(char const * buf, size_t len, write_handler handler) {
		m_alog->write(log::alevel::devel,"orbis_con async_write");
		m_bufs.push_back(buffer(buf, len));

		try_write(0, 0, handler);
	}

	/// Asynchronous Transport Write (scatter-gather)
	/**
	 * Write a sequence of buffers to the output stream. Call handler to report
	 * success or failure. handler may or may not be called during async_write,
	 * but it must be safe for this to happen.
	 *
	 * Will return 0 on success.
	 *
	 * @param bufs vector of buffers to write
	 * @param handler Callback to invoke with operation status.
	 */
	void async_write(std::vector<buffer> const & bufs, write_handler handler) {
		m_alog->write(log::alevel::devel,"orbis_con async_write buffer list");
		for (const auto& buf : bufs) {
			m_bufs.push_back(buf);
		}

		try_write(0, 0, handler);
	}

	void try_write(size_t start_index, size_t start_buffer_offset, write_handler handler) {
		lib::error_code ec;
		size_t bytesSent = start_buffer_offset;
		bool finished = true;

		for (size_t i = start_index; i < m_bufs.size(); ++i) {
			const auto& buf = m_bufs[i];
			int sceResult = 0;

			// Write as much as we can
			while (bytesSent < buf.len) {
				sceResult = sceSslWrite(m_ssl_con, buf.buf+bytesSent, buf.len-bytesSent, 0);
				if (sceResult > 0) {
					bytesSent += sceResult;
				}
				else {
					break;
				}
			}

			// Schedule a wait and keep track of which buffer and how far into the buffer we need to continue
			if (sceResult == SCE_SSL_ERROR_WANT_POLLIN || sceResult == SCE_SSL_ERROR_WANT_POLLOUT) {
				auto eventType = (sceResult == SCE_SSL_ERROR_WANT_POLLIN
					? websocketpp::orbis::epoll_service::event_type::pollin
					: websocketpp::orbis::epoll_service::event_type::pollout);
				if (!m_epoll_service->addCallback(eventType, m_socket,
					lib::bind(&type::try_write, get_shared(), i, bytesSent, handler)))
				{
					m_elog->write(log::elevel::rerror, "epoll_service::addCallback() failed");
					ec = make_error_code(error::write);
				}
				else {
					finished = false;
				}

				break;
			}
			else if (sceResult < 0) {
				log_sce_err(log::elevel::rerror, "sceSslWrite() failed", sceResult);
				ec = make_error_code(error::write);
				break;
			}

			bytesSent = 0;
		}

		if (ec || finished) {
			m_bufs.clear();
			handler(ec);
		}
	}

	/// Set Connection Handle
	/**
	 * @param hdl The new handle
	 */
	void set_handle(connection_hdl hdl) {
		m_handle = hdl;
	}

	/// Call given handler back within the transport's event system (if present)
	/**
	 * Invoke a callback within the transport's event system if it has one. If
	 * it doesn't, the handler will be invoked immediately before this function
	 * returns.
	 *
	 * @param handler The callback to invoke
	 *
	 * @return Whether or not the transport was able to register the handler for
	 * callback.
	 */
	lib::error_code dispatch(dispatch_handler handler) {
		handler();
		return lib::error_code();
	}

	/// Perform cleanup on socket shutdown_handler
	/**
	 * @param h The `shutdown_handler` to call back when complete
	 */
	void async_shutdown(shutdown_handler handler) {
		lib::error_code ec;
		int sceResult = 0;
		if (m_ssl_con >= 0) {
			sceResult = sceSslDeleteConnection(m_ssl_con);
			if (sceResult < 0) {
				log_sce_err(log::elevel::rerror, "sceSslDeleteConnection() failed", sceResult);
				ec = make_error_code(error::shutdown);
			}
			m_ssl_con = -1;
		}

		if (m_socket >= 0) {
			m_epoll_service->deregisterDescriptor(m_socket);
			sceResult = sceNetSocketClose(m_socket);
			if (sceResult < 0) {
				log_sce_err(log::elevel::rerror, "sceNetSocketClose() failed", sceResult);
				ec = make_error_code(error::shutdown);
			}
			m_socket = -1;
		}

		handler(ec);
	}

private:
	/// Convenience method for logging the code and message for an error_code
	void log_sce_err(log::level l, const char * msg, int error) {
		std::stringstream s;
		s << msg << " sce_error: 0x" << std::setfill('0') << std::setw(8) << std::hex << error;
		m_elog->write(l, s.str());
	}

	// member variables!
	lib::shared_ptr<alog_type> m_alog;
	lib::shared_ptr<elog_type> m_elog;

	std::vector<buffer> m_bufs;

	lib::shared_ptr<websocketpp::orbis::epoll_service> m_epoll_service;

	connection_hdl m_handle;
	uri_ptr m_uri;

	SceNetId m_socket{-1};
	int m_ssl_con{-1};
};


} // namespace orbis
} // namespace transport
} // namespace websocketpp
