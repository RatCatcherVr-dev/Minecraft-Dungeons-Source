#pragma once

#include <websocketpp/common/system_error.hpp>
#include <websocketpp/common/cpp11.hpp>

#include <string>

namespace websocketpp {
namespace transport {
namespace orbis {

// Forward declaration of class endpoint so that it can be friended/referenced
// before being included.
template <typename config>
class endpoint;

/// orbis transport errors
namespace error {
enum value {
	/// Catch-all error for transport policy errors that don't fit in other
	/// categories
	general = 1,

	/// sceNetResolverStartNtoa() error
	resolver,

	/// error during connection
	connect,

	/// error during ssl connection
	ssl_connect,

	/// sceSslRead() error
	read,

	/// sceSslWrite() error
	write,

	/// shutdown error
	shutdown
};

/// orbis transport error category
class category : public lib::error_category {
	public:
	category() {}

	char const * name() const _WEBSOCKETPP_NOEXCEPT_TOKEN_ {
		return "websocketpp.transport.orbis";
	}

	std::string message(int value) const {
		switch(value) {
			case general:
				return "Generic orbis transport policy error";
			case resolver:
				return "DNS resolver failure";
			case connect:
				return "Connect failure";
			case ssl_connect:
				return "SSL connect failure";
			case read:
				return "Read failure";
			case write:
				return "Write failure";
			case shutdown:
				return "Shutdown failure";
			default:
				return "Unknown";
		}
	}
};

/// Get a reference to a static copy of the orbis transport error category
inline lib::error_category const & get_category() {
	static category instance;
	return instance;
}

/// Get an error code with the given value and the orbis transport category
inline lib::error_code make_error_code(error::value e) {
	return lib::error_code(static_cast<int>(e), get_category());
}

} // namespace error
} // namespace orbis
} // namespace transport
} // namespace websocketpp
_WEBSOCKETPP_ERROR_CODE_ENUM_NS_START_
template<> struct is_error_code_enum<websocketpp::transport::orbis::error::value>
{
	static bool const value = true;
};
_WEBSOCKETPP_ERROR_CODE_ENUM_NS_END_
