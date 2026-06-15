#pragma once

#include "Array.h"
#include "UnrealString.h"

class UBtNode;

struct UBtLogNode {
	enum class WillRunResult {
		NotRun, True, False
	};
	FString name;
	bool isRunning;
	WillRunResult willRunResult;
	TArray<UBtLogNode> children;
};

namespace logger {
	void ConsoleLogger(bool isRunning, UBtLogNode::WillRunResult, int, bool hasChildren, const FString&);

	class ScreenLogger {
		int offset { 0 };
	public:
		ScreenLogger() = default;
		void operator()(bool isRunning, UBtLogNode::WillRunResult, int, bool hasChildren, const FString&);
	};
}

class UBtNodeLogger {
public:
	template <typename T>
	static void Log(T&& log, const UBtNode& node) {
		Log(log, GenerateTree(node), 0);
	}

	static void LogScreen(const UBtNode& node) {
		Log(logger::ScreenLogger {}, node);
	}

	static void LogConsole(const UBtNode& node) {		 
		Log(logger::ConsoleLogger, node);
	}

private:
	static UBtLogNode GenerateTree(const UBtNode&);

	template <typename T>
	static void Log(T&& log, const UBtLogNode&, int indent);
};

template <typename T>
void UBtNodeLogger::Log(T&& log, const UBtLogNode& node, int indent) {	
	log(
		node.isRunning,
		node.willRunResult,
		indent,
		node.children.Num() > 0,
		node.name
	);

	for (auto&& child : node.children) {
		Log(log, child, indent + 1);
	}
}