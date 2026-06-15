#pragma once

namespace filter {

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