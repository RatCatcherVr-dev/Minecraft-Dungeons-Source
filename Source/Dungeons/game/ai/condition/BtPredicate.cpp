#include "Dungeons.h"
#include "BtPredicate.h"

UBtPredicate::UBtPredicate(const bt::Pred& predicate)
	: UBtPredicate(predicate, bt::Pred {}) {
}

UBtPredicate::UBtPredicate(const bt::Pred& canRun, const bt::Pred& canContinueToRun)
	: predicate(canRun)
	, stopPredicate(canContinueToRun ? canContinueToRun : TOptional<bt::Pred>{}) {
	type = "predicate";
}

bool UBtPredicate::OnCanRun(bt::StateRef state) {
	return predicate(state);
}

bool UBtPredicate::OnCanContinue(bt::StateRef state) {
	return stopPredicate ? stopPredicate.GetValue()(state) : predicate(state);
}
