#pragma once

#include "TeamName.h"
#include "TeamRelation.h"
#include <Map.h>

struct TeamDef {
	TeamDef(ETeamName name);

	ETeamRelation getRelationTowards(ETeamName) const;

protected:
	TMap<ETeamName, ETeamRelation> mRelations;	

private:
	ETeamName mName;
};

