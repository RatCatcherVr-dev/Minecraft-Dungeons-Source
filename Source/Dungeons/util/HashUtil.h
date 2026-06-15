#pragma once

#include <type_traits>
#include <Optional.h>

#define REDIRECT_HASH(T) \
namespace std { \
template <> struct hash<T> { \
	std::size_t operator()(const T& value) const { return value.hashCode(); } \
}; \
}

namespace std {

template <typename T>
struct hash<TOptional<T>> {
	std::size_t operator()(const TOptional<T>& value) const {
		return value.IsSet() ? std::hash<T>{}(value.GetValue()) : 0;
	}
};

}

template <typename T>
inline void hash_combine(std::size_t& combined, const T& value) {
	combined = 1009 * combined + std::hash<T>{}(value);
}
