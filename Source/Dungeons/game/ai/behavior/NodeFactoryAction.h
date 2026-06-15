#pragma once
#include "game/ai/bt/BtTypes.h"

class UBtNode;
using NodeAction = std::function<void(UBtNode&)>;

NodeAction Name(const FString& name);
NodeAction AppendName(const FString& name, const FString& separator = ":");
