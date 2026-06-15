#pragma once

namespace bt {
// Impl

template <typename Expression, typename ...Providers>
auto exp(const Expression& fun, const Providers&... provider) {
	return [=](StateRef state) {
		return fun(provider(state)...);
	};
}
	
//
// Boolean operators
//
template <typename Predicate>
auto logicalNot(const Predicate& predicate) {
	return [=](StateRef state) {
		return !predicate(state);
	};
}

template <typename PredicateA, typename PredicateB>
auto logicalAnd(const PredicateA& a, const PredicateB& b) {
	return [=](StateRef state) { return a(state) && b(state); };
}

template <typename Predicate>
auto logicalAnd(std::initializer_list<Predicate> terms) {
	return[v = std::vector<Predicate>(begin(terms), end(terms))](StateRef state) {
		return std::all_of(begin(v), end(v), [&state](const Predicate& p) { return p(state); });
	};
}

template <typename PredicateA, typename PredicateB>
auto logicalOr(const PredicateA& a, const PredicateB& b) {
	return [=](StateRef state) { return a(state) || b(state); };
}

template <typename Predicate>
auto logicalOr(std::initializer_list<Predicate> terms) {
	return[v = std::vector<Predicate>(begin(terms), end(terms))](StateRef state) {
		return std::any_of(begin(v), end(v), [&state](const Predicate& p) { return p(state); });
	};
}

//
// Comparisons
//
template <typename ProviderA, typename ProviderB>
auto equals(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa == bb;
	};
}

template <typename Provider>
auto equals(std::initializer_list<Provider> terms) {
	return[v = std::vector<Provider>(begin(terms), end(terms))](StateRef state) {
		return std::adjacent_find(begin(v), end(v), [&state](const Provider& a, const Provider& b) {
			return a(state) != b(state); }) == end(v);
	};
}

template <typename ProviderA, typename ProviderB>
auto lessThan(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa < bb;
	};
}

template <typename ProviderA, typename ProviderB>
auto lessThanOrEquals(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa <= bb; // @todo if needed: aa < bb || aa == bb
	};
}

template <typename ProviderA, typename ProviderB>
auto lessThanOrNearlyEquals(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa < bb || FMath::IsNearlyEqual(aa, bb, 0.01f); // @todo if needed: aa < bb || aa == bb
	};
}

template <typename ProviderA, typename ProviderB>
auto greaterThan(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa > bb; // @todo if needed: aa != bb && !(aa < bb)
	};
}

template <typename ProviderA, typename ProviderB>
auto greaterThanOrEquals(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa >= bb;
	};
}

template <typename ProviderA, typename ProviderB>
auto greaterThanOrNearlyEquals(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) {
		auto&& aa = a(state);
		auto&& bb = b(state);
		return aa > bb || FMath::IsNearlyEqual(aa, bb, 0.01f);
	};
}

template <typename ProviderA>
auto isZero(const ProviderA& provider) {
	return [=](StateRef state) {
		return provider(state) == 0;
	};
}


//
// Arithmetic
//
template <typename ProviderA, typename ProviderB>
auto plus(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) { return a(state) + b(state); };
}

template <typename ProviderA, typename ProviderB>
auto minus(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) { return a(state) - b(state); };
}

template <typename ProviderA, typename ProviderB>
auto times(const ProviderA& a, const ProviderB& b) {
	return [=](StateRef state) { return a(state) * b(state); };
}

template <typename ProviderA, typename ProviderB>
Action plusAssign(const ProviderA& ref, const ProviderB& value) {
	return [=](StateRef state) { ref(state, ref(state) + value(state)); };
}

template <typename ProviderA, typename ProviderB>
Action minusAssign(const ProviderA& ref, const ProviderB& value) {
	return [=](StateRef state) { ref(state, ref(state) - value(state)); };
}

template <typename ProviderA, typename ProviderB>
Action timesAssign(const ProviderA& ref, const ProviderB& value) {
	return [=](StateRef state) { ref(state, ref(state) * value(state)); };
}

template <typename GetSet>
Action inc(const GetSet& ref) {
	return [=](StateRef state) { ref(state, ref(state) + 1); };
}

template <typename GetSet>
Action dec(const GetSet& ref) {
	return [=](StateRef state) { ref(state, ref(state) - 1); };
}

//
// Misc
//
template <typename Provider>
auto firstValue(const Provider& a, const Provider& b) {
	return [=](StateRef state) {
		if (const auto&& aa = a(state)) {
			return aa;
		}

		return b(state);
	};
}

template <typename Provider>
auto firstValue(std::initializer_list<Provider> terms) {
	return [v = std::vector<Provider>(begin(terms), end(terms))](StateRef state) {
		const auto result = std::find_if(begin(v), end(v), [&state](const Provider& p) { return p(state); });
		
		if (result == end(v)) {
			return nullptr;
		}

		return result;
	};
}

template <typename T>
auto retry(const bt::Provider<T>& provider, int triesMax) {
	return [=](StateRef state) -> T {
		for (auto tries = 0; tries < triesMax; tries++) {
			if (const auto maybeValue = provider(state)) {
				return maybeValue;
			}
		}

		return {};
	};
}

template <typename Predicate, typename ProviderA, typename ProviderB>
auto choice(const Predicate& predicate, const ProviderA& providerA, const ProviderB& providerB) {
	return [=] (StateRef state) {
		return predicate(state) ? providerA(state) : providerB(state);
	};
}

}
