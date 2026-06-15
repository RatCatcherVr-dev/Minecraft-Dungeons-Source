#include "Dungeons.h"
#include "Engine.h"
#include "BtNode.h"
#include "group/BtGroup.h"
#include "LogMacros.h"
#include "BtNodeLogger.h"

namespace logger {
	void ConsoleLogger(bool isRunning, UBtLogNode::WillRunResult willRunResult, int indent, bool hasChildren, const FString& arg) {
		auto argFormatted = arg;
		auto red = false;

		if (isRunning) {
			if (willRunResult == UBtLogNode::WillRunResult::False) {
				argFormatted += " (X)";
			}
		} else {
			if (willRunResult == UBtLogNode::WillRunResult::True) {
				argFormatted += " - true";
				red = true;
			}
			if (willRunResult == UBtLogNode::WillRunResult::False) {
				argFormatted += " (X)";
				red = true;
			}
		}

		auto padding = FString::ChrN(indent * 4 + (red ? 2 : 0), TCHAR(' ')); // +2 meaning: len(Display) - len(Error)

		argFormatted = padding + argFormatted;
		if (hasChildren) {
			argFormatted += ":";
		}

		if (isRunning) {
			UE_LOG(LogDungeonsAI, Warning, TEXT("%s"), *argFormatted);
		} else if (red) {
			UE_LOG(LogDungeonsAI, Error, TEXT("%s"), *argFormatted);
		} else {
			UE_LOG(LogDungeonsAI, Display, TEXT("%s"), *argFormatted);
		}
	}

	void ScreenLogger::operator()(bool isRunning, UBtLogNode::WillRunResult willRunResult, int indent, bool hasChildren, const FString& arg) {
		const auto color = [&] {
			if (isRunning) {
				switch (willRunResult) {
				case UBtLogNode::WillRunResult::True:
					return FColor::Yellow;
				default:
					return FColor { 255, 128, 128 };
				}
			} else {
				switch (willRunResult) {
				case UBtLogNode::WillRunResult::True:
					return FColor { 128, 128, 0 };
				case UBtLogNode::WillRunResult::False:
					return FColor::Red;
				default:
					return FColor::White;
				}
			}
		}();
		

		const auto indentation = FString::ChrN(indent * 4, TCHAR(' '));
		const auto text = FString::Printf(TEXT("%s%s%s"), *indentation, *arg, hasChildren? ":" : " ");
		GEngine->AddOnScreenDebugMessage(offset, 5.f, color, text, true);

		offset++;
	}	
}

UBtLogNode UBtNodeLogger::GenerateTree(const UBtNode& node) {
	const auto currentTick = bt::internal::currentTickId;

	UBtLogNode root;
	root.name = node.ToString();
	root.isRunning = node.IsRunning();
	root.willRunResult = (node.lastWillRunTick.value == currentTick)
		? (node.willRun ? UBtLogNode::WillRunResult::True : UBtLogNode::WillRunResult::False)
		: (UBtLogNode::WillRunResult::NotRun);

	if (node.isGroup) {
		auto& group = static_cast<const UBtGroup&>(node);
		for (auto& child : group.children) {
			root.children.Add(GenerateTree(*child));
		}
	}
	return root;
}

