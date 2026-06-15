/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

namespace Util {

struct PairKeyFunctor {
	template<typename T>
	typename T::first_type operator()(T& pair) const {
		return pair.first;
	}

};

struct PairValueFunctor {
	template<typename T>
	typename T::second_type operator()(T& pair) const {
		return pair.second;
	}

};

template <class T1, class T2>
TPair<T1, T2> makeTPair(T1 v1, T2 v2) {
	return TPair<T1, T2>(std::move(v1), std::move(v2));
}

template <typename ElemType>
TArray<ElemType> createZeroedTArrayOfSize(size_t size) {
	TArray<ElemType> out;
	out.AddZeroed(size);
	return std::move(out);
}

template <typename ElemType>
TArray<ElemType> createDefaultedTArrayOfSize(size_t size) {
	TArray<ElemType> out;
	out.AddDefaulted(size);
	return std::move(out);
}

template <typename ElemType>
TArray<ElemType> createUninitializedTArrayOfSize(size_t size) {
	TArray<ElemType> out;
	out.AddUninitialized(size);
	return std::move(out);
}

template <typename ElemType>
TArray<ElemType> createTArrayOfReservedSize(size_t size) {
	TArray<ElemType> out;
	out.Reserve(size);
	return std::move(out);
}

template <typename ElemType>
void removeNullsSwap(TArray<ElemType>& values, bool allowShrinking = true) {
	values.RemoveAllSwap([](const ElemType& elem) { return elem == nullptr; }, allowShrinking);
}

template <typename ElemType>
TArray<ElemType> subsequence(const TArray<ElemType>& collection, int start, int end = std::numeric_limits<int>::max()) {
	end = std::min(end, collection.Num());
	check(start >= 0 && start <= end);
	auto out = createTArrayOfReservedSize<ElemType>(end - start);
	for (int i = start; i < end; ++i) {
		out.Add(collection[i]);
	}
	return out;
}

template <typename Transform, typename ElemType>
auto map(const Transform& transform, const TArray<ElemType>& collection, int start = 0, int end = std::numeric_limits<int>::max()) -> TArray<decltype(transform(collection.Top()))> {
	start = std::min(std::max(0, start), collection.Num());
	end = std::min(end, collection.Num());
	auto out = createTArrayOfReservedSize<decltype(transform(collection.Top()))>(end - start);
	for (int i = start; i < end; ++i) {
		out.Add(transform(collection[i]));
	}
	return out;
}

template <typename Transform, typename Iter>
auto map(const Transform& transform, Iter first, const Iter& last) -> TArray<decltype(transform(*first))> {
	auto out = createTArrayOfReservedSize<decltype(transform(*first))>(std::distance(first, last));
	for (; first != last; ++first) {
		out.Add(transform(*first));
	}
	return out;
}

template <typename T>
TOptional<T> single(const std::vector<T>& values) {
	return values.size() == 1 ? *values.begin() : TOptional<T>{};
}

template <typename T>
TOptional<T> single(const TArray<T>& vs) {
	return vs.Num() == 1 ? vs[0] : TOptional<T>{};
}

template <typename T>
bool isValidIndex(const std::vector<T>& collection, unsigned int index) {
	return index < collection.size();
}

template <typename T>
static TArray<T*> getRawPointers(const TArray<TWeakObjectPtr<T>>& weakPointers) {
	TArray<T*> pointers;
	for (auto&& weak : weakPointers) {
		if (auto ptr = weak.Get()) {
			pointers.Add(ptr);
		}
	}
	return pointers;
}


}
