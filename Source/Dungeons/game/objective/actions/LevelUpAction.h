#pragma once

#include "game/objective/ObjectiveAction.h"

namespace game { namespace objective {

class LevelUpAction: public ObjectiveAction {
public:
	LevelUpAction(int level);

	void onStop() override;
private:
	int mLevel;
};

}}
