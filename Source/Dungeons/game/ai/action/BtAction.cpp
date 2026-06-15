#include "Dungeons.h"
#include "BtAction.h"

UBtAction::UBtAction(const bt::Action& action)
	: actions(1) {
	actions[0] = action;
	type = "action";
}

UBtAction::UBtAction(std::initializer_list<bt::Action> actions)
	: actions(actions) {
	type = "action";
}

void UBtAction::CallActions(bt::StateRef state) {
	for (auto&& action : actions) {
		action(state);
	}
}

Unique<UBtNode> onStart(const bt::Action& action) {
	return make_unique<UBtStartAction>(action);
}

Unique<UBtNode> onStart(std::initializer_list<bt::Action> actions) {
	return make_unique<UBtStartAction>(actions);
}

Unique<UBtNode> onTick(const bt::Action& action) {
	return make_unique<UBtTickAction>(action);
}

Unique<UBtNode> onTick(std::initializer_list<bt::Action> actions) {
	return make_unique<UBtTickAction>(actions);
}

Unique<UBtNode> onStop(const bt::Action& action) {
	return make_unique<UBtStopAction>(action);
}

Unique<UBtNode> onStop(std::initializer_list<bt::Action> actions) {
	return make_unique<UBtStopAction>(actions);
}
