#pragma once
#include "BtTypes.h"

namespace bt {

template <typename Expression, typename ...Providers>
auto exp(const Expression& fun, const Providers&... provider);


//
// Boolean operators
//
template <typename Predicate>
auto logicalNot(const Predicate&);

template <typename PredicateA, typename PredicateB>
auto logicalAnd(const PredicateA&, const PredicateB&);

template <typename Predicate>
auto logicalAnd(std::initializer_list<Predicate>);

template <typename PredicateA, typename PredicateB>
auto logicalOr(const PredicateA&, const PredicateB&);

template <typename Predicate>
auto logicalOr(std::initializer_list<Predicate>);

Pred operator!(const Pred&);
Pred operator&&(const Pred&, const Pred&);
Pred operator||(const Pred&, const Pred&);

//
// Comparisons
//
template <typename ProviderA, typename ProviderB>
auto equals(const ProviderA&, const ProviderB&);

template <typename Provider>
auto equals(std::initializer_list<Provider>);

template <typename ProviderA, typename ProviderB>
auto notEquals(const ProviderA& a, const ProviderB& b) { return not(equals(a, b)); }

template <typename ProviderA, typename ProviderB>
auto lessThan(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto lessThanOrEquals(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto lessThanOrNearlyEquals(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto greaterThan(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto greaterThanOrEquals(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto greaterThanOrNearlyEquals(const ProviderA&, const ProviderB&);

template <typename ProviderA>
auto isZero(const ProviderA&);


//
// Arithmetic
//
template <typename ProviderA, typename ProviderB>
auto plus(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto minus(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
auto times(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
Action plusAssign(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
Action minusAssign(const ProviderA&, const ProviderB&);

template <typename ProviderA, typename ProviderB>
Action timesAssign(const ProviderA&, const ProviderB&);

template <typename GetSet>
Action inc(const GetSet&);

template <typename GetSet>
Action dec(const GetSet&);

//
// Assignment
//
template <typename Setter, typename Provider>
auto set(const Setter& setter, const Provider& provider) {
	return [=](StateRef state) { setter(state, provider(state)); };
}

template <typename Setter>
auto set(const Setter& setter, int value) {
	return [=](StateRef state) { setter(state, value); };
}

template <typename Setter>
auto set(const Setter& setter, bool value) {
	return [=](StateRef state) { setter(state, value); };
}

//
// Misc
//
template <typename Provider>
auto firstValue(const Provider&, const Provider&);

template <typename Provider>
auto firstValue(std::initializer_list<Provider>);


template <typename T>
auto retry(const bt::Provider<T>& provider, int triesMax);


template <typename Predicate, typename ProviderA, typename ProviderB>
auto choice(const Predicate&, const ProviderA&, const ProviderB&);

}

#include "BtLogic.hpp"
