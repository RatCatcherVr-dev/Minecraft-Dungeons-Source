#pragma once

#include "TeamName.h"
#include "TeamRelation.h"

namespace teamquery {
	namespace {
		ETeamRelation GetTeamRelationTowards(ETeamName instigatorTeam, ETeamName targetTeam);		
	}
	namespace is {
		bool friendly(ETeamName instigatorTeam, ETeamName targetTeam);
		bool neutral(ETeamName instigatorTeam, ETeamName targetTeam);
		bool hostile(ETeamName instigatorTeam, ETeamName targetTeam);
	}

	namespace can {
		DUNGEONS_API bool damage(ETeamName instigatorTeam, ETeamName targetTeam);
		             bool heal(ETeamName instigatorTeam, ETeamName targetTeam);
	}
};