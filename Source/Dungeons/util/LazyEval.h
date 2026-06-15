#pragma once

#include <utility>
#include <functional>

namespace lazyeval {

namespace internal {

template <typename T, typename Derived>
class Base {
public:
	Base(T value = {}) : mValue(std::move(value)) {}

	      T& operator*()       { return get(); }
	const T& operator*() const { return get(); }

	      T* operator->()      { return &get(); }
	const T* operator->() const{ return &get(); }
private:
	T& get() const;
protected:
	void reset() const;
	mutable T mValue;
private:
	mutable bool mHasValue = false;
};

}

//
// See Base above for public interface
//

#define DEFINE_FUNCTION_OBJECT_BASED_TYPE(Name, Func, Generate) \
template <typename T>                                  \
class Name : public internal::Base<T, Name<T>> {       \
	using base = internal::Base<T, Name<T>>;           \
public:                                                \
	Name(Func f = {}, T value = {}) : base(std::move(value)), f(std::move(f)) {} \
protected:                                             \
	friend base;                                       \
	void generateValue() const { if (f) { Generate } } \
	Func f;                                            \
};

DEFINE_FUNCTION_OBJECT_BASED_TYPE(OutParameter0, std::function<void(T&)>,
	f(this->mValue);
)
DEFINE_FUNCTION_OBJECT_BASED_TYPE(OutParameterRefreshable0, std::function<bool(T&)>,
	if (!f(this->mValue)) { this->reset(); };
)
DEFINE_FUNCTION_OBJECT_BASED_TYPE(Provider0, std::function<T()>,
	this->mValue = f();
)
DEFINE_FUNCTION_OBJECT_BASED_TYPE(Provider1, std::function<T(const T&)>,
	this->mValue = f(this->mValue);
)

}

#include "LazyEval.hpp"
