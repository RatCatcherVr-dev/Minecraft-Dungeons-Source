#pragma once

#include "game/ai/bt/BtEvalState.h"

namespace bt {

template <typename Pred, typename Action>
auto ifAction(Pred pred, Action action) {
	return [=](StateRef state) {
		if (pred(state)) {
			action(state);
		}
	};
}

template <typename Pred, typename IfAction, typename ElseAction>
auto ifElseAction(Pred pred, IfAction ifAction, ElseAction elseAction) {
	return [=](StateRef state) {
		if (pred(state)) {
			ifAction(state);
		} else {
			elseAction(state);
		}
	};
}

}
