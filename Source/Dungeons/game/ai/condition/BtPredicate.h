#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "CommonTypes.h"

class UBtPredicate: public UBtLeaf {
public:
	UBtPredicate(const bt::Pred&);
	UBtPredicate(const bt::Pred& canRun, const bt::Pred& canContinueToRun);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
private:
	bt::Pred predicate;
	TOptional<bt::Pred> stopPredicate;
};


template <typename CanRun>
Unique<UBtPredicate> predicate(CanRun canRun) {
	return make_unique<UBtPredicate>(std::forward<CanRun>(canRun));
}

template <typename CanRun>
Unique<UBtPredicate> startPredicate(CanRun canRun) {
	return make_unique<UBtPredicate>(std::forward<CanRun>(canRun), [](bt::StateRef) { return true; });
}

template <typename CanRun, typename CanContinueToRun>
Unique<UBtPredicate> startContinuePredicate(CanRun canRun, CanContinueToRun canContinueToRun) {
	return make_unique<UBtPredicate>(std::forward<CanRun>(canRun), std::forward<CanContinueToRun>(canContinueToRun));
}
