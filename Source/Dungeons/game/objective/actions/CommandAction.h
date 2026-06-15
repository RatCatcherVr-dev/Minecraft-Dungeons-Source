#pragma once

#include "game/objective/ObjectiveAction.h"

namespace game { namespace objective {

class CommandAction: public ObjectiveAction {
public:
	void addStartCommand(FString);
	void addStopCommand(FString);

	void onStart() override;
	void onStop() override;
private:
	void _runCommands(const TArray<FString>&);

	TArray<FString> mStartCommands;
	TArray<FString> mStopCommands;
};

}}
