#include "Dungeons.h"
#include "TeamDef.h"


TeamDef::TeamDef(ETeamName name) : mName(name) {}

ETeamRelation TeamDef::getRelationTowards(ETeamName team) const
{
	if (mRelations.Contains(team)) {
		return mRelations[team];
	}

	return ETeamRelation::Neutral;
}