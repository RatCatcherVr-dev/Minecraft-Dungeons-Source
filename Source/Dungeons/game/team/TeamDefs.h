#pragma once

#include "TeamDef.h"

namespace teams {
	const TeamDef& get(ETeamName team);
	const TeamDef* getChecked(ETeamName team);
}