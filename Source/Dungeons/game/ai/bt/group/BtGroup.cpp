#include "Dungeons.h"
#include "BtGroup.h"
#include "game/ai/action/BtAction.h"
#include "game/ai/condition/BtPredicate.h"

UBtGroup::UBtGroup() {
	isGroup = true;
	type = "group";
}

void UBtGroup::AddChild(Unique<UBtNode> node) {
	if (!node) {
		return;
	}
	checkf(node->parent == nullptr, TEXT("Node already got a parent!"));
	node->parent = this;	
	children.push_back(std::move(node));
}

void UBtGroup::OnStop(bt::StateRef state) {
	for (auto& child : children) {
		if (child->IsRunning()) {
			child->Stop(state);
		}
	}
}

void UBtGroup::OnInitialise(bt::StateRef state) {
	for (auto& child : children) 
	{		//Initialise children nodes
			child->_Initialise(state);
	}
}


void handleParam(UBtGroup& node, const bt::Action& action) {
	node.AddChild(onTick(action));
}

void handleParam(UBtGroup& node, const bt::Pred& pred) {
	node.AddChild(predicate(pred));
}

void handleParam(UBtGroup& node, std::initializer_list<bt::Action> actions) {
	node.AddChild(onTick(actions));
}