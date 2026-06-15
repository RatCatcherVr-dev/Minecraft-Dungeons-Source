#include "Dungeons.h"
#include "TeamDefs.h"
#include "util/CollectionUtils.h"
#include "CommonTypes.h"

namespace teams {

	struct MutableTeamDef : TeamDef {
		using TeamDef::TeamDef;

		MutableTeamDef& hostileTowards(ETeamName team) { 
			mRelations.Add(team, ETeamRelation::Hostile); 
			return *this; 
		}

		MutableTeamDef& friendlyTowards(ETeamName team) {
			mRelations.Add(team, ETeamRelation::Friendly); 
			return *this;
		}

		MutableTeamDef& neutralTowards(ETeamName team) {
			mRelations.Add(team, ETeamRelation::Neutral); 
			return *this;
		}
	};

	auto teamDefs = Util::createDefaultedTArrayOfSize<Unique<MutableTeamDef>>(enum_cast(ETeamName::World) + 1);

	MutableTeamDef& create(ETeamName teamName) {
		const int index = enum_cast(teamName);
		teamDefs[index] = make_unique<MutableTeamDef>(teamName);
		return *teamDefs[index];
	}


/*
	TEAMS!!!
*/
	const TeamDef& Loner = create(ETeamName::Loner)
		.hostileTowards(ETeamName::Loner)
		.hostileTowards(ETeamName::Heroes)
		.hostileTowards(ETeamName::Keys)
		.hostileTowards(ETeamName::World)
		.hostileTowards(ETeamName::Villains)
		.hostileTowards(ETeamName::WorldLoner);

	const TeamDef& Heroes = create(ETeamName::Heroes)
		.friendlyTowards(ETeamName::Heroes)
		.neutralTowards(ETeamName::Keys)
		.neutralTowards(ETeamName::World)
		.hostileTowards(ETeamName::Villains)
		.hostileTowards(ETeamName::Loner)
		.neutralTowards(ETeamName::WorldLoner);

	const TeamDef& Villains = create(ETeamName::Villains)
		.friendlyTowards(ETeamName::Villains)
		.neutralTowards(ETeamName::World)
		.hostileTowards(ETeamName::Heroes)
		.hostileTowards(ETeamName::Keys)
		.neutralTowards(ETeamName::Loner)
		.neutralTowards(ETeamName::WorldLoner);

	const TeamDef& World = create(ETeamName::World)
		.friendlyTowards(ETeamName::World)
		.neutralTowards(ETeamName::Heroes)
		.neutralTowards(ETeamName::Keys)
		.neutralTowards(ETeamName::Villains)
		.neutralTowards(ETeamName::Loner)
		.neutralTowards(ETeamName::WorldLoner);

	const TeamDef& Keys = create(ETeamName::Keys)
		.friendlyTowards(ETeamName::Keys)
		.neutralTowards(ETeamName::Heroes)
		.neutralTowards(ETeamName::Villains)
		.neutralTowards(ETeamName::World)
		.neutralTowards(ETeamName::Loner)
		.neutralTowards(ETeamName::WorldLoner);

	const TeamDef& WorldLoner = create(ETeamName::WorldLoner)
		.neutralTowards(ETeamName::WorldLoner)
		.neutralTowards(ETeamName::Keys)
		.neutralTowards(ETeamName::Heroes)
		.hostileTowards(ETeamName::Villains)
		.neutralTowards(ETeamName::World)
		.neutralTowards(ETeamName::Loner);

	//Getters

	const TeamDef& get(ETeamName team) {
		if (auto Team = getChecked(team)) {
			return *Team;
		}
		ensure(false && "All teams should have a definition");
		return World;
	}

	const TeamDef* getChecked(ETeamName team) {
		const int index = static_cast<int>(team);
		if (index >= 0 && index < teamDefs.Num()) {
			return teamDefs[index].get();
		}
		return nullptr;
	}

}