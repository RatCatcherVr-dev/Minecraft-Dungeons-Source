#include "Dungeons.h"
#include "CommandAction.h"
#include "DungeonsGameInstance.h"
#include "game/Game.h"
#include <HAL/ConsoleManager.h>

namespace game { namespace objective {

void CommandAction::addStartCommand(FString command) {
	mStartCommands.Add(std::move(command));
}

void CommandAction::addStopCommand(FString command) {
	mStopCommands.Add(std::move(command));
}

void CommandAction::onStart() {
	_runCommands(mStartCommands);
}

void CommandAction::onStop() {
	_runCommands(mStopCommands);
}

void CommandAction::_runCommands(const TArray<FString>& commands) {
	FOutputDevice& out = *GLog;

	for (auto& cmd : commands) {
		FConsoleManager::Get().ProcessUserConsoleInput(*cmd, out, &game().world());
	}
}

}}
