#pragma once

template <typename T>
using Pred = std::function<bool(T)>;

//D11.PS - had to rename this namespace boolean is a key word on XB1
namespace booleanOperator {

template <class ElemType>
Pred<ElemType> And(const Pred<ElemType>& pred1, const Pred<ElemType>& pred2) {
	return [=](ElemType e) {
		return pred1(e) && pred2(e);
	};
}

template <class ElemType>
Pred<ElemType> Or(const Pred<ElemType>& pred1, const Pred<ElemType>& pred2) {
	return [=](ElemType e) {
		return pred1(e) || pred2(e);
	};
}

template <class ElemType>
Pred<ElemType> Not(const Pred<ElemType>& pred) {
	return [=](ElemType e) {
		return !pred(e);
	};
}

}

#define GENERATE_BOOLEAN_PRED_AND(T) inline Pred<T> operator&&(const Pred<T>& pred1, const Pred<T>& pred2) { return booleanOperator::And<T>(pred1, pred2); }
#define GENERATE_BOOLEAN_PRED_OR(T)  inline Pred<T> operator||(const Pred<T>& pred1, const Pred<T>& pred2) { return booleanOperator::Or<T>(pred1, pred2); }
#define GENERATE_BOOLEAN_PRED_NOT(T) inline Pred<T> operator!(const Pred<T>& pred) { return booleanOperator::Not<T>(pred); }

#define GENERATE_BOOLEAN_PRED_OPERATORS(T)  \
GENERATE_BOOLEAN_PRED_AND(T) \
GENERATE_BOOLEAN_PRED_NOT(T) \
GENERATE_BOOLEAN_PRED_OR(T)
