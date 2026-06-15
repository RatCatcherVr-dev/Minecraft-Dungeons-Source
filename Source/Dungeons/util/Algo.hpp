#pragma once

#include "TypeTraits.h"
#include "Random.h"
#include "SharedRandom.h"

namespace algo {

using std::begin;
using std::end;

template <typename T>
auto copyable_begin(T& t) { return begin(t); }
template <typename T>
auto copyable_begin(const T& t) { return begin(t); }
template <typename E>
auto copyable_begin(TArray<E>& t) { return t.GetData(); }
template <typename E>
auto copyable_begin(const TArray<E>& t) { return t.GetData(); }

template <typename T>
auto copyable_end(T& t) { return end(t); }
template <typename T>
auto copyable_end(const T& t) { return end(t); }
template <typename E>
auto copyable_end(TArray<E>& t) { return t.GetData() + t.Num(); }
template <typename E>
auto copyable_end(const TArray<E>& t) { return t.GetData() + t.Num(); }

template <typename T>
void append(std::vector<T>& collection, const T& value) { collection.push_back(value); }
template <typename T>
void append(std::vector<T>& collection, T&& value) { collection.push_back(std::move(value)); }
template <typename T>
void append(std::unordered_set<T>& collection, const T& value) { collection.insert(value); }
template <typename T>
void append(std::unordered_set<T>& collection, T&& value) { collection.insert(std::move(value)); }
template <typename T, typename Cmp>
void append(std::set<T, Cmp>& collection, const T& value) { collection.insert(value); }
template <typename T, typename Cmp>
void append(std::set<T, Cmp>& collection, T&& value) { collection.insert(std::move(value)); }
template <typename K, typename V>
void append(std::unordered_map<K, V>& collection, const std::pair<K, V>& value) { collection.insert(value); }
template <typename K, typename V>
void append(std::unordered_map<K, V>& collection, std::pair<K, V>&& value) { collection.insert(std::move(value)); }
template <typename K, typename V, typename Cmp>
void append(std::map<K, V, Cmp>& collection, const std::pair<K, V>& value) { collection.insert(value); }
template <typename K, typename V, typename Cmp>
void append(std::map<K, V, Cmp>& collection, std::pair<K, V>&& value) { collection.insert(std::move(value)); }
template <typename T>
void append(TArray<T>& collection, const T& value) { collection.Add(value); }
template <typename T>
void append(TArray<T>& collection, T&& value) { collection.Add(MoveTemp(value)); }
template <typename K, typename V>
void append(TMap<K, V>& collection, const typename TMap<K, V>::ElementType& value) { collection.Add(value); }
template <typename K, typename V>
void append(TMap<K, V>& collection, typename TMap<K, V>::ElementType&& value) { collection.Add(MoveTemp(value)); }
template <typename K>
void append(TSet<K>& collection, const typename TSet<K>::ElementType& value) { collection.Add(value); }
template <typename K>
void append(TSet<K>& collection, typename TSet<K>::ElementType&& value) { collection.Add(MoveTemp(value)); }

template <typename Collection>
struct element_type {
	using type = remove_cvref_t<decltype(*copyable_begin(std::declval<Collection>()))>;
};
template <typename Collection>
using element_type_t = typename element_type<Collection>::type;


template<typename Collection, typename Pred>
bool all_of(const Collection& collection, const Pred& p) {
	const auto last = copyable_end(collection);
	return std::find_if_not(copyable_begin(collection), last, p) == last;
}

template<typename Collection, typename Pred>
bool any_of(const Collection& collection, const Pred& p) {
	const auto last = copyable_end(collection);
	return std::find_if(copyable_begin(collection), last, p) != last;
}

template<typename Collection, typename Pred>
bool none_of(const Collection& collection, const Pred& p) {
	const auto last = copyable_end(collection);
	return std::find_if(copyable_begin(collection), last, p) == last;
}

namespace internal {

template <bool ScoreByMax, typename Collection, typename Scorer>
auto score_element_by(const Collection& collection, Scorer scorer) {
	using ElementType = remove_cvref_t<decltype(*copyable_begin(collection))>;	

	if (copyable_begin(collection) == copyable_end(collection)) {
		return TOptional<ElementType>{};
	}
	
	auto iterator = copyable_begin(collection);
	auto iteratorBest = iterator;
	auto scoreBest = scorer(*iterator);

	iterator++;

	const auto end = copyable_end(collection);
	for (; iterator != end; iterator++) {
		const auto scoreCandidate = scorer(*iterator);
		if (ScoreByMax? scoreCandidate > scoreBest : scoreCandidate < scoreBest) {
			iteratorBest = iterator;
			scoreBest = scoreCandidate;
		}
	}

	return TOptional<ElementType> { *iteratorBest };
}

template <typename Collection, class T, class AccumulatorElementOperation>
T accumulate(const Collection& collection, T init, const AccumulatorElementOperation& op) {
	for (const auto& element : collection) {
		init = op(std::move(init), element);
	}
	return init;
}

#define accumulate_unordered accumulate

}

template<typename Collection, typename T>
T sum_with_init(const Collection& collection, T init) {
	const auto last = end(collection);
	for (auto first = begin(collection); first != last; ++first) {
		init = init + *first;
	}
	return init;
}

template<typename Collection, typename Transform, typename T>
T sum_with_init(const Collection& collection, const Transform& transform, T init) {
	const auto last = end(collection);
	for (auto first = begin(collection); first != last; ++first) {
		init = init + transform(*first);
	}
	return init;
}

template<typename Collection, typename Transform>
auto sum(const Collection& collection, const Transform& transform) {
	return sum_with_init<Collection, Transform, remove_cvref_t<decltype(transform(*begin(collection)))>>(collection, transform, {});
}

template<typename Collection>
auto sum(const Collection& collection) {
	return sum_with_init<Collection, remove_cvref_t<decltype(*begin(collection))>>(collection, 0);
}

template<typename Collection, typename Pred>
int count_if(const Collection& collection, const Pred& pred) {
	return internal::accumulate_unordered(collection, 0, [&pred](int count, auto& e) { return count + (pred(e) ? 1 : 0); });
}

template<typename Collection, typename Value>
int count(const Collection& collection, const Value& value) {
	return count_if(collection, RETLAMBDA(it == value));
}

template <typename Collection>
auto min_element(const Collection& collection) {
	return std::min_element(copyable_begin(collection), copyable_end(collection));
}

template <typename Collection, typename Pred>
auto min_element(const Collection& collection, Pred pred) {
	return std::min_element(copyable_begin(collection), copyable_end(collection), pred);
}

template <typename Collection>
auto max_element(const Collection& collection) {
	return std::max_element(copyable_begin(collection), copyable_end(collection));
}

template <typename Collection, typename Pred>
auto max_element(const Collection& collection, Pred pred) {
	return std::max_element(copyable_begin(collection), copyable_end(collection), pred);
}


template <typename Collection, typename Scorer>
auto min_element_by(const Collection& collection, Scorer scorer) {
	return internal::score_element_by<false>(collection, scorer);
}

template <typename Collection, typename Scorer>
auto max_element_by(const Collection& collection, Scorer scorer) {
	return internal::score_element_by<true>(collection, scorer);
}

template <typename Collection, typename Pred>
size_t copy_if(const Collection& collection, const Pred& pred, Collection& out) {
	size_t count = 0;
	for (const auto& e : collection) {
		if (pred(e)) {
			append(out, e);
			count++;
		}
	}
	return count;
}

template <typename Collection, typename Pred>
Collection copy_if(const Collection& collection, const Pred& pred) {
	Collection out;
	copy_if(collection, pred, out);
	return out;
}

template <typename Collection, typename Pred, typename Transform, typename OutCollection>
size_t copy_if_map(const Collection& in, const Pred& pred, const Transform& f, OutCollection& out) {
	size_t count = 0;
	for (const auto& e : in) {
		if (pred(e)) {
			append(out, f(e));
			count++;
		}
	}
	return count;
}

template <typename OutCollection, typename Collection, typename Pred, typename Transform>
auto copy_if_map_as(const Collection& collection, const Pred& pred, const Transform& f) {
	OutCollection out;
	copy_if_map(collection, pred, f, out);
	return out;
}

template <typename Collection, typename Pred, typename Transform>
auto copy_if_map_tarray(const Collection& collection, const Pred& pred, const Transform& f) {
	return copy_if_map_as<TArray<decltype(f(*copyable_begin(collection)))>>(collection, pred, f);
}

template <typename Collection, typename Pred, typename Transform>
auto copy_if_map_vector(const Collection& collection, const Pred& pred, const Transform& f) {
	return copy_if_map_as<std::vector<decltype(f(*copyable_begin(collection)))>>(collection, pred, f);
}

template <typename Collection, typename Transform, typename Pred, typename OutCollection>
size_t map_if(const Collection& in, const Transform& f, const Pred& pred, OutCollection& out) {
	size_t count = 0;
	for (const auto& e : in) {
		auto mapped = f(e);

		const auto& mappedConst = mapped;
		if (pred(mappedConst)) {
			append(out, std::move(mapped));
			count++;
		}
	}
	return count;
}

template <typename OutCollection, typename Collection, typename Transform, typename Pred>
auto map_if_as(const Collection& collection, const Transform& f, const Pred& pred) {
	OutCollection out;
	map_if(collection, f, pred, out);
	return out;
}

template <typename Collection, typename Transform, typename Pred>
auto map_if_tarray(const Collection& collection, const Transform& f, const Pred& pred) {
	return map_if_as<TArray<decltype(f(*copyable_begin(collection)))>>(collection, f, pred);
}

template <typename Collection, typename Transform, typename Pred>
auto map_if_vector(const Collection& collection, const Transform& f, const Pred& pred) {
	return map_if_as<std::vector<decltype(f(*copyable_begin(collection)))>>(collection, f, pred);
}

template <typename Collection, typename Transform, typename OutCollection>
void map_to(const Collection& in, const Transform& f, OutCollection& out) {
	for (const auto& e : in) {
		append(out, f(e));
	}
}

template <typename OutCollection, typename Collection, typename Transform>
auto map_as(const Collection& collection, const Transform& f) {
	OutCollection out;
	map_to(collection, f, out);
	return out;
}

template <typename Collection, typename Transform>
auto map_tarray(const Collection& collection, const Transform& f) {
	return map_as<TArray<decltype(f(*copyable_begin(collection)))>>(collection, f);
}

template <typename Collection, typename Transform>
auto map_vector(const Collection& collection, const Transform& f) {
	return map_as<std::vector<decltype(f(*copyable_begin(collection)))>>(collection, f);
}

template <typename Collection, typename OutCollection>
auto map_cast_to(const Collection& collection, OutCollection& out) {
	using OutT = decltype(*copyable_begin(std::declval<OutCollection>()));
	map_to(collection, RETLAMBDA(static_cast<OutT>(it)), out);
}

template <typename OutCollection, typename Collection>
auto map_cast(const Collection& collection) {
	OutCollection out;
	map_cast_to(collection, out);
	return out;
}

template <typename Collection, typename Transform, typename OutCollection>
void flatmap_to(const Collection& in, const Transform& f, OutCollection& out) {
	for (const auto& e : in) {
		map_cast_to(f(e), out);
	}
}

template <typename Collection, typename Transform>
auto flatmap(const Collection& in, const Transform& f) {
	std::remove_reference_t<decltype(f(*copyable_begin(in)))> out;
	flatmap_to(in, f, out);
	return out;
}

template <typename OutCollection, typename Collection, typename Transform>
auto flatmap_as(const Collection& collection, const Transform& f) {
	OutCollection out;
	flatmap_to(collection, f, out);
	return out;
}

template <typename Collection, typename Transform>
auto flatmap_tarray(const Collection& collection, const Transform& f) {
	using TransformCollectionType = std::remove_reference_t<decltype(f(*copyable_begin(collection)))>;
	return flatmap_as<TArray<element_type_t<TransformCollectionType>>>(collection, f);
}

template <typename Collection, typename Transform>
auto flatmap_vector(const Collection& collection, const Transform& f) {
	using TransformCollectionType = std::remove_reference_t<decltype(f(*copyable_begin(collection)))>;
	return flatmap_as<std::vector<element_type_t<TransformCollectionType>>>(collection, f);
}

template <typename Collection, typename Value>
bool contains(const Collection& collection, const Value& value) {
	const auto last = copyable_end(collection);
	return std::find(copyable_begin(collection), last, value) != last;
}

template <typename Collection, typename Value>
bool add_unique(Collection& collection, const Value& value) {
	if (!contains(collection, value)) {
		append(collection, value);
		return true;
	}
	return false;
}

template <typename Collection, typename Value, typename Pred>
bool add_unique_if(Collection& collection, const Value& value, const Pred& pred) {
	if (none_of(collection, pred)) {
		append(collection, value);
		return true;
	}
	return false;
}

template <typename DstCollection, typename SrcCollection>
void append_all(DstCollection& dst, const SrcCollection& src) { algo::map_cast_to(src, dst); }

template <typename Collection, typename Fn>
void for_each(Collection&& collection, const Fn& fn) {
	for (auto& e : collection) {
		fn(e);
	}
}

template <typename Collection, typename Value>
TOptional<int> index_of(const Collection& collection, const Value& value) {
	int i = 0;
	for (const auto& e : collection) {
		if (e == value) {
			return i;
		}
		++i;
	}
	return {};
}

template <typename Collection, typename Pred>
TOptional<int> index_of_if(const Collection& collection, const Pred& pred) {
	int i = 0;
	for (const auto& e : collection) {
		if (pred(e)) {
			return i;
		}
		++i;
	}
	return {};
}

template <typename OutCollection>
void iota_to(int start, int endExclusive, OutCollection& out) {
	ensure(start <= endExclusive);

	using T = std::remove_reference_t<decltype(*copyable_begin(out))>;
	for (T i = T{ start }; i < T{ endExclusive }; i += T{ 1 }) {
		append(out, i);
	}
}

template <typename T>
std::vector<T> iota_vector(int start, int endExclusive) {
	std::vector<T> vs;
	vs.reserve(endExclusive - start);
	iota_to(start, endExclusive, vs);
	return vs;
}
template <typename T>
std::vector<T> iota_vector(int n) {
	return iota_vector<T>(0, n);
}

template <typename T>
TArray<T> iota_tarray(int start, int endExclusive) {
	TArray<T> vs;
	vs.Reserve(endExclusive - start);
	iota_to(start, endExclusive, vs);
	return vs;
}
template <typename T>
TArray<T> iota_tarray(int n) {
	return iota_tarray<T>(0, n);
}

template <typename Collection, typename LessOf>
Collection unique_by_less_of(const Collection& collection, const LessOf& lessOf) {
	using ComparisonType = remove_cvref_t<decltype(lessOf(*copyable_begin(collection)))>;
	std::set<ComparisonType> unique;

	Collection out;

	for (const auto& e : collection) {
		if (unique.insert(lessOf(e)).second) {
			append(out, e);
		}
	}
	return out;
}


namespace random {
//
// Random
//

template <typename Collection>
void shuffle(Collection& collection, Random& rnd) {
	const auto size = copyable_end(collection) - copyable_begin(collection);
	if (size <= 1) {
		return;
	}
	for (auto i = size - 1; i > 0; --i) {
		using std::swap;
		swap(collection[i], collection[rnd.nextInt(i + 1)]);
	}
}

template <typename Collection>
void shuffle(Collection& collection) {
	shuffle(collection, Util::sharedRandom());
}

template <typename Collection>
Collection shuffledCopy(const Collection& collection, Random& rnd) {
	Collection copy = collection;
	shuffle(copy, rnd);
	return copy;
}

}

namespace opt {
//
// Optional
//

template <typename Optionals, typename OutCollection>
auto unpack(const Optionals& collection, OutCollection& out) {
	return copy_if_map(collection, RETLAMBDA(it), RETLAMBDA(it.GetValue()), out);
}

template <typename OutCollection, typename Optionals>
auto unpack_as(const Optionals& collection) {
	return copy_if_map_as<OutCollection>(collection, RETLAMBDA(it), RETLAMBDA(std::move(it.GetValue())));
}

template <typename Optionals>
auto unpack_vector(const Optionals& collection) {
	return copy_if_map_vector(collection, RETLAMBDA(it), RETLAMBDA(it.GetValue()));
}

template <typename Optionals>
auto unpack_tarray(const Optionals& collection) {
	return copy_if_map_tarray(collection, RETLAMBDA(it), RETLAMBDA(it.GetValue()));
}

template <typename T>
TOptional<T> firstValid(const TOptional<T>& a, const TOptional<T>& b) {
	return a ? a : b;
}

template <typename Optionals>
auto firstValid(const Optionals& collection) {
	using OptionalT = remove_cvref_t<decltype(*copyable_begin(collection))>;

	for (auto& option : collection) {
		if (option) {
			return option;
		}
	}
	return OptionalT{};
}

template <typename T, typename ComputeFunction>
T get_or_compute(const TOptional<T>& optional, const ComputeFunction& compute) {
	if (optional) {
		return optional.GetValue();
	}
	return compute();
}

template <typename T, typename ComputeFunction>
T& get_or_set_compute(TOptional<T>& optional, const ComputeFunction& compute) {
	if (!optional) {
		optional = compute();
	}
	return optional.GetValue();
}

}

#undef accumulate_unordered

}
