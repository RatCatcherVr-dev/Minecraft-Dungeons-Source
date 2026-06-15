#pragma once

#include "game/ai/bt/BtEvalState.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/behavior/NodeFactoryAction.h"

namespace bt { namespace random {

Pred probability(float prob);
Pred probability(const Provider<float>& prob);

}}


template <typename Prob>
Unique<UBtNode> probability(Prob prob) {
	return meta(predicate(bt::random::probability(prob)), Name("probability"));
}

template <typename Prob, typename Task>
Unique<UBtNode> probability(Prob prob, Task&& task) {
	return sequence("condition", probability(prob), std::forward<Task>(task));
}

template <typename Prob>
Unique<UBtNode> startProbability(Prob prob) {
	return meta(startPredicate(bt::random::probability(prob)), Name("start-probability"));
}

template <typename Prob, typename Task>
Unique<UBtNode> startProbability(Prob prob, Task&& task) {
	return sequence("condition", startProbability(prob), std::forward<Task>(task));
}

template <typename StartProb, typename ContinueProb, typename Task>
Unique<UBtNode> startContinueProbability(StartProb startProb, ContinueProb continueProb) {
	return meta(startContinuePredicate(bt::random::probability(startProb), bt::random::probability(continueProb)), Name("start-continue-probability"));
}

template <typename StartProb, typename ContinueProb, typename Task>
Unique<UBtNode> startContinueProbability(StartProb startProb, ContinueProb continueProb, Task&& task) {
	return sequence("condition", startContinueProbability(startProb, continueProb), std::forward<Task>(task));
}
