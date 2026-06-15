#include "Dungeons.h"
#include "TeamQuery.h"
#include "TeamDefs.h"

namespace teamquery {

	namespace {
		ETeamRelation GetTeamRelationTowards(ETeamName instigatorTeam, ETeamName targetTeam)
		{
			return teams::get(instigatorTeam).getRelationTowards(targetTeam);
		}
	}
	
	namespace is {		
		bool friendly(ETeamName instigatorTeam, ETeamName targetTeam)
		{
			return GetTeamRelationTowards(instigatorTeam, targetTeam) == ETeamRelation::Friendly;			
		}

		bool neutral(ETeamName instigatorTeam, ETeamName targetTeam)
		{
			return GetTeamRelationTowards(instigatorTeam, targetTeam) == ETeamRelation::Neutral;
		}

		bool hostile(ETeamName instigatorTeam, ETeamName targetTeam)
		{
			return GetTeamRelationTowards(instigatorTeam, targetTeam) == ETeamRelation::Hostile;		
		}
	}

	namespace can {
		//Damage can be dealt towards hostile and neutral teams.
		bool damage(ETeamName instigatorTeam, ETeamName targetTeam) {
			return is::hostile(instigatorTeam, targetTeam) || is::neutral(instigatorTeam, targetTeam);
		}

		//Healing can be done to teammates and friendly teams.
		bool heal(ETeamName instigatorTeam, ETeamName targetTeam) {
			return instigatorTeam == targetTeam || is::friendly(instigatorTeam, targetTeam);
		}
	}
}