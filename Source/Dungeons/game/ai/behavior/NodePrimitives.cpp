#include "Dungeons.h"
#include "NodePrimitives.h"
#include "game/ai/bt/BtNode.h"

Unique<UBtNode> alwaysTrue() {
	class AlwaysTrue : public UBtNode {
	protected:
		bool OnWillRun(bt::StateRef) override {
			return true;
		}
	};
	return make_unique<AlwaysTrue>();
}

Unique<UBtNode> alwaysFalse() {
	class AlwaysFalse : public UBtNode {
	protected:
		bool OnWillRun(bt::StateRef) override {
			return false;
		}
	};
	return make_unique<AlwaysFalse>();
}
