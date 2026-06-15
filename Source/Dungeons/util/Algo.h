#pragma once

#include <vector>
#include "TypeTraits.h"
#include "Algo.hpp"

class Random;

namespace algo {

template<typename Collection, typename Pred>
bool all_of(const Collection&, const Pred&);

template<typename Collection, typename Pred>
bool any_of(const Collection&, const Pred&);

template<typename Collection, typename Pred>
bool none_of(const Collection&, const Pred&);

template<typename Collection, typename T>
T sum_with_init(const Collection&, T init);

template<typename Collection, typename Transform, typename T>
T sum_with_init(const Collection&, const Transform&, T init);

template<typename Collection, typename Transform>
auto sum(const Collection&, const Transform&);

template<typename Collection>
auto sum(const Collection&);

template<typename Collection, typename Value>
int count(const Collection&, const Value&);

template<typename Collection, typename Pred>
int count_if(const Collection&, const Pred&);

template <typename Collection>
auto min_element(const Collection&);

template <typename Collection, typename Pred>
auto min_element(const Collection&, Pred);

template <typename Collection>
auto max_element(const Collection&);

template <typename Collection, typename Pred>
auto max_element(const Collection&, Pred);

template <typename Collection, typename Scorer>
auto min_element_by(const Collection&, Scorer);

template <typename Collection, typename Scorer>
auto max_element_by(const Collection&, Scorer);

template <typename Collection, typename Pred>
size_t copy_if(const Collection&, const Pred&, Collection& out);

template <typename Collection, typename Pred>
Collection copy_if(const Collection&, const Pred&);

template <typename Collection, typename Pred, typename Transform, typename OutCollection>
size_t copy_if_map(const Collection&, const Pred&, const Transform&, OutCollection&);

template <typename OutCollection, typename Collection, typename Pred, typename Transform>
auto copy_if_map_as(const Collection&, const Pred&, const Transform&);

template <typename Collection, typename Pred, typename Transform>
auto copy_if_map_tarray(const Collection&, const Pred&, const Transform&);

template <typename Collection, typename Pred, typename Transform>
auto copy_if_map_vector(const Collection&, const Pred&, const Transform&);

template <typename Collection, typename Transform, typename Pred, typename OutCollection>
size_t map_if(const Collection&, const Transform&, const Pred&, OutCollection&);

template <typename OutCollection, typename Collection, typename Transform, typename Pred>
auto map_if_as(const Collection&, const Transform&, const Pred&);

template <typename Collection, typename Transform, typename Pred>
auto map_if_tarray(const Collection&, const Transform&, const Pred&);

template <typename Collection, typename Transform, typename Pred>
auto map_if_vector(const Collection&, const Transform&, const Pred&);

template <typename Collection, typename Transform, typename OutCollection>
void map_to(const Collection&, const Transform&, OutCollection&);

template <typename OutCollection, typename Collection, typename Transform>
auto map_as(const Collection&, const Transform&);

template <typename Collection, typename Transform>
auto map_tarray(const Collection&, const Transform&);

template <typename Collection, typename Transform>
auto map_vector(const Collection&, const Transform&);

template <typename Collection, typename OutCollection>
auto map_cast_to(const Collection&, OutCollection&);

template <typename OutCollection, typename Collection>
auto map_cast(const Collection&);

template <typename Collection, typename Transform, typename OutCollection>
void flatmap_to(const Collection&, const Transform&, OutCollection&);

template <typename Collection, typename Transform>
auto flatmap(const Collection&, const Transform&);

template <typename OutCollection, typename Collection, typename Transform>
auto flatmap_as(const Collection&, const Transform&);

template <typename Collection, typename Transform>
auto flatmap_tarray(const Collection&, const Transform&);

template <typename Collection, typename Transform>
auto flatmap_vector(const Collection&, const Transform&);

template <typename Collection, typename Value>
bool contains(const Collection&, const Value&);

template <typename Collection, typename Value>
bool add_unique(Collection&, const Value&);

template <typename Collection, typename Value, typename Pred>
bool add_unique_if(Collection&, const Value&, const Pred&);

template <typename DstCollection, typename SrcCollection>
void append_all(DstCollection&, const SrcCollection&);

template <typename Collection, typename Fn>
void for_each(Collection&&, const Fn&); // std::for_each actually returns Fn, but let's add it if we need it

template <typename Collection, typename Value>
TOptional<int> index_of(const Collection&, const Value&);

template <typename Collection, typename Pred>
TOptional<int> index_of_if(const Collection&, const Pred&);

template <typename OutCollection>
void iota_to(int start, int endExclusive, OutCollection&);

template <typename T = int>
std::vector<T> iota_vector(int start, int endExclusive);

template <typename T = int>
std::vector<T> iota_vector(int n);

template <typename T = int>
TArray<T> iota_tarray(int start, int endExclusive);

template <typename T = int>
TArray<T> iota_tarray(int n);

template <typename Collection, typename LessOf>
Collection unique_by_less_of(const Collection&, const LessOf&);


namespace random {
//
// Random
//
template <typename Collection>
void shuffle(Collection&);

template <typename Collection>
void shuffle(Collection&, Random&);

template <typename Collection>
Collection shuffledCopy(const Collection&, Random&);

}

namespace opt {
//
// Optional
//
template <typename Optionals, typename OutCollection>
auto unpack(const Optionals&, OutCollection&);

template <typename OutCollection, typename Optionals>
auto unpack_as(const Optionals&);

template <typename Optionals>
auto unpack_vector(const Optionals&);

template <typename Optionals>
auto unpack_tarray(const Optionals&);

template <typename T>
TOptional<T> firstValid(const TOptional<T>&, const TOptional<T>&);

template <typename Optionals>
auto firstValid(const Optionals&);

template <typename T, typename ComputeFunction>
T get_or_compute(const TOptional<T>&, const ComputeFunction&);

template <typename T, typename ComputeFunction>
T& get_or_set_compute(const TOptional<T>&, const ComputeFunction&);

}

namespace generate {

template <typename G, typename P>
auto generate_while(G generator, P predicate) -> std::vector<decltype(generator(0U))> {
	std::vector<decltype(generator(0U))> values;
	for (size_t i = 0;; ++i) {
		auto value = generator(i);
		if (!predicate(i, value)) {
			break;
		}
		values.push_back(value);
	}
	return values;
}

template <typename G>
auto generate_n(size_t n, G generator) -> std::vector<decltype(generator(0U))> {
	std::vector<decltype(generator(0U))> values;
	values.reserve(n);

	for (size_t i = 0; i < n; ++i) {
		values.push_back(generator(i));
	}
	return values;
}

namespace predicates {
	template <typename T>
	auto count(size_t size) {
		return [size](size_t index, const T&) { return index < size; };
	}

	template <typename T>
	auto valueLessThanOrEquals(const T& maxValue) {
		return [maxValue](size_t index, const T& value) { return value <= maxValue; };
	}

	template <typename T>
	auto differenceLessThanOrEquals(const T& maxDifference) {
		return [maxDifference, last = 0] (size_t index, const T& value) mutable {
			auto difference = value - last;
			last = value;
			return index == 0 ? true : difference <= maxDifference;
		};
	}
}

}}
