#pragma once

#include <type_traits>

//
// Remove reference, const and volatile
//
template<class T>
struct remove_cvref {
	typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template<class T>
using remove_cvref_t = typename remove_cvref<T>::type;

//
// Test whether the given type T has a T::value member
//
//D11.PS - Switch & PS4 do not like this...
#if !PLATFORM_SWITCH && !PLATFORM_PS4
template <class, class = std::void_t<>>
struct has_value: std::false_type {};

template <class T>
struct has_value<T, std::void_t<decltype(T::value)>>: std::true_type {};
#endif
