#pragma once

#include "BtEvalState.h"
#include "CommonTypes.h"

namespace bt {

template <typename T>
struct Ref {
	Ref(T& val): value(val) {}

	const T& operator()(StateRef) const {
		return value;
	}
	void operator()(StateRef, const T& val) const {
		value = val;
	}
	T& operator*()  const {
		return value;
	}
	T* operator->() const {
		return &value;
	}
	operator T&() const {
		return value;
	}
private:
	T& value;
};


template <typename T>
struct UObjectRef {
	UObjectRef(T* val) : value(val) {}

	T* operator()(StateRef) const {
		return value.Get();
	}
	void operator()(StateRef, const T* val) const {
		value = val;
	}
	T& operator*()  const {
		return *value;
	}
	T* operator->() const {
		return value.Get();
	}
	operator T&() const {
		return *value;
	}
private:
	mutable TWeakObjectPtr<T> value;
};


template <typename T>
struct SharedRef {
	SharedRef(Shared<T> val) : ptr(val) {}
	SharedRef(const SharedRef& rhs) : ptr(rhs.ptr) {}

	const T& operator()(StateRef) const {
		return *ptr;
	}
	void operator()(StateRef, const T& val) const {
		*ptr = val;
	}
	T& operator*()  const {
		return *ptr;
	}
	T* operator->() const {
		return ptr.get();
	}
	operator T&() const {
		return *ptr;
	}
private:
	Shared<T> ptr;
};

template <typename T>
struct SharedObjectRef {
	SharedObjectRef(T* val = nullptr) : ptr(make_shared<TWeakObjectPtr<T>>(val)) {}
	SharedObjectRef(const SharedObjectRef& rhs) : ptr(rhs.ptr) {}

	T* operator()(StateRef) const {
		return ptr->Get();
	}
	void operator()(StateRef, const T* val) const {
		*ptr = val;
	}
	T& operator*()  const {
		return **ptr;
	}
	T* operator->() const {
		return ptr->Get();
	}
	operator T*() const {
		return ptr->Get();
	}
private:
	Shared<TWeakObjectPtr<T>> ptr;
};

template <typename T>
typename std::enable_if< std::is_base_of<UObject, T>::value, UObjectRef<T>>::type
ref(T* value) {
	return UObjectRef<T>(value);
}

template <typename T>
typename std::enable_if< !std::is_base_of<UObject, T>::value, Ref<T>>::type
ref(T& value) {
	return Ref<T>(value);
}

// Creates a shared_ptr of type <T> with value = value. e.g. sharedRef(true) -> ... make_shared<bool>(true);
template <typename T>
SharedRef<T> sharedRef(T&& value) {
	return SharedRef<T>(make_shared<T>(std::forward<T>(value)));
}

template <typename T>
typename std::enable_if< std::is_base_of<UObject, T>::value, SharedObjectRef<T>>::type
sharedObjectRef(T* init = nullptr) {
	return SharedObjectRef<T>(init);
}

template <typename T, typename ...Args>
SharedRef<T> makeSharedRef(Args&&... args) {
	return SharedRef<T>(make_shared<T>(std::forward<Args>(args)...));
}

}
