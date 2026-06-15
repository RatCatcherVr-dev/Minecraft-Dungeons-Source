#include "Dungeons.h"
#include "NodeFactoryAction.h"
#include "game/ai/bt/BtNode.h"

NodeAction Name(const FString& name) {
	return [name](auto& node) { node.name = name; };
}

NodeAction AppendName(const FString& name, const FString& separator/*= ":"*/) {
	return [name, separator](auto& node) { node.name += separator + name; };
}

