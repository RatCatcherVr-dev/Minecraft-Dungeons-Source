#pragma once

#include "NodeFactoryAction.h"
#include "NodePrimitives.h"
#include "game/ai/bt/BtNode.h"
#include "game/ai/bt/group/BtGroup.h"
#include "game/ai/bt/group/BtSequence.h"
#include "game/ai/bt/group/BtChain.h"
#include "game/ai/bt/group/BtSelector.h"
#include "game/ai/bt/group/BtRandomSelector.h"
#include "game/ai/bt/group/BtParallel.h"
#include "game/ai/bt/group/BtGreedySequence.h"
#include "game/ai/bt/group/BtIfElse.h"
#include "game/ai/bt/group/BtStartIfElse.h"
#include "game/ai/bt/BtCondition.h"
#include "game/ai/bt/BtRoot.h"
#include "game/ai/bt/BtUninterruptible.h"
#include "game/ai/action/BtAction.h"
#include "game/ai/condition/TimeLimited.h"
#include "CommonTypes.h"

void handleParam(UBtGroup&, Unique<UBtNode>&& child);
void handleParam(UBtNode&, const NodeAction&);
void handleParam(UBtNode&, const FString& name);

template <class NodeType, class nothing = void>
void handleParams(NodeType&) {}

template <typename NodeType, typename ParamType, typename ...Rest>
void handleParams(NodeType& node, ParamType param, Rest&&... rest) {
	handleParam(node, std::forward<ParamType>(param));
	handleParams<NodeType, Rest...>(node, std::forward<Rest>(rest)...);
}

template <typename Node, typename ...Rest>
Node&& meta(Node&& node, Rest&&... rest) {
	handleParams(*node, std::forward<Rest>(rest)...);
	return std::move(node);
}

template <typename T, typename ...Tasks>
Unique<T> createGroup(Tasks&&... tasks) {
	return meta(make_unique<T>(), std::forward<Tasks>(tasks)...);
}

template <typename T>
Unique<T> ifAdd(bool condition, Unique<T> If) {
	return condition ? std::move(If) : Unique<T>{};
}

template <typename T>
T&& ifElseAdd(bool condition, T&& If, T&& Else) {
	return condition ? std::forward<T>(If) : std::forward<T>(Else);
}

//
// Node factory methods
//
template <typename ...Tasks>
Unique<UBtSequence> sequence(Tasks&&... tasks) {
	return createGroup<UBtSequence>(std::forward<Tasks>(tasks)...);
}

template <typename ...Tasks>
Unique<UBtChain> chain(Tasks&&... tasks) {
	return createGroup<UBtChain>(std::forward<Tasks>(tasks)...);
}

template <typename ...Tasks>
Unique<UBtGreedySequence> greedySequence(Tasks&&... tasks) {
	return createGroup<UBtGreedySequence>(std::forward<Tasks>(tasks)...);
}

template <typename ...Tasks>
Unique<UBtSelector> selector(Tasks&&... tasks) {
	return createGroup<UBtSelector>(std::forward<Tasks>(tasks)...);
}

template <typename ...Tasks>
Unique<UBtRandomSelector> randomSelector(Tasks&&... tasks) {
	return createGroup<UBtRandomSelector>(std::forward<Tasks>(tasks)...);
}

template <typename ...Tasks>
Unique<UBtParallel> parallel(Tasks&&... tasks) {
	return createGroup<UBtParallel>(std::forward<Tasks>(tasks)...);
}

template <typename Condition, typename IfTrue, typename IfFalse>
Unique<UBtIfElse> ifElse(Condition condition, IfTrue ifTrue, IfFalse ifFalse) {
	return createGroup<UBtIfElse>(std::forward<Condition>(condition), std::forward<IfTrue>(ifTrue), std::forward<IfFalse>(ifFalse));
}

template <typename Condition, typename IfTrue, typename IfFalse>
Unique<UBtStartIfElse> startIfElse(Condition condition, IfTrue ifTrue, IfFalse ifFalse) {
	return createGroup<UBtStartIfElse>(std::forward<Condition>(condition), std::forward<IfTrue>(ifTrue), std::forward<IfFalse>(ifFalse));
}

template <typename CanRun>
Unique<UBtCondition> condition(CanRun canRun) {
	return createGroup<UBtCondition>(std::forward<CanRun>(canRun));
}

template <typename CanRun>
Unique<UBtCondition> startCondition(CanRun canRun) {
	return createGroup<UBtCondition>(std::forward<CanRun>(canRun), alwaysTrue());
}

template <typename CanRun, typename CanContinueToRun>
Unique<UBtCondition> startContinueCondition(CanRun canRun, CanContinueToRun canContinueToRun) {
	return createGroup<UBtCondition>(std::forward<CanRun>(canRun), std::forward<CanContinueToRun>(canContinueToRun));
}

template <typename Task>
Unique<UBtRoot> root(Task task) {
	return createGroup<UBtRoot>(std::forward<Task>(task));
}

template <typename Task>
Unique<UBtRoot> uninterruptibleRoot(Task task) {
	return createGroup<UBtRoot>(std::forward<Task>(task));
}

template <typename Task>
Unique<UBtUninterruptible> uninterruptible(Task&& task) {
	return createGroup<UBtUninterruptible>(std::forward<Task>(task));
}

template <typename ...Tasks>
Unique<UBtUninterruptible> uninterruptibleSequence(Tasks&&... tasks) {
	return uninterruptible(createGroup<UBtSequence>(std::forward<Tasks>(tasks)...));
}

Unique<UBtUninterruptible> wait(bt::Duration duration);

template <typename Task>
Unique<UBtGroup> alwaysTrue(Task&& task) {
	return parallel("always-true",
		std::forward<Task>(task),
		alwaysTrue()
	);
}
