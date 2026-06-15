#include "Dungeons.h"
#include "NodeFactory.h"

void handleParam(UBtGroup& node, Unique<UBtNode>&& child) {
	node.AddChild(std::move(child));
}

void handleParam(UBtNode& node, const NodeAction& action) {
	action(node);
}

void handleParam(UBtNode& node, const FString& name) {
	node.SetName(name);
}

Unique<UBtUninterruptible> wait(bt::Duration duration) {
	return meta(uninterruptible(maxRunTime(duration)), "wait");
}
