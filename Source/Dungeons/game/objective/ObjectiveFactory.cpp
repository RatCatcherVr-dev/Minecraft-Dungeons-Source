#include "Dungeons.h"
#include "ObjectiveFactory.h"
#include "ObjectiveAction.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "game/objective/actions/CommandAction.h"
#include "game/objective/actions/LevelUpAction.h"
#include "game/objective/objectives/ArenaBattle.h"
#include "game/objective/objectives/ClickPuzzle.h"
#include "game/objective/objectives/SequentialClickPuzzle.h"
#include "game/objective/objectives/Gauntlet.h"
#include "game/objective/objectives/KillGroup.h"
#include "game/objective/objectives/SoundData.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "util/StringUtil.h"
#include "actions/SpawnItemWhenPlayerRunsOutOf.h"

namespace game { namespace objective {

Unique<SoundData> createSoundData(const TOptional<io::ObjectiveSoundData>& sound) {
	if (!sound) {
		return {};
	}
	return make_unique<SoundData>(sound.GetValue());
} 

Pair<FString, FString> splitCommandAndRest(FString s) {
	s.TrimStartAndEndInline();

	const auto i = s.Find(" ");
	if (i == INDEX_NONE) {
		return { s, {} };
	}
	return { s.Left(i).TrimEnd(), s.RightChop(i + 1).TrimStart() };
}

Unique<ObjectiveAction> metaToAction(const std::string& s) {
	const auto commandAndRest = splitCommandAndRest(stringutil::toFString(s));
	auto& cmd = commandAndRest.first;
	auto& rest = commandAndRest.second;

	// The rest, as a list (of words/arguments)
	TArray<FString> args;
	rest.ParseIntoArray(args, TEXT(" "));

	if (cmd == "level-up" && rest == "2") {
		return make_unique<LevelUpAction>(ArgAsInt(rest).Get(2));
	}
	if (cmd == "spawn-arrows") {
		auto region = ArgAsFString(args, 0).Get("*.*.spawn_arrow");
		return make_unique<SpawnItemWhenPlayerRunsOutOf>(stringutil::toStdString(region), game::item::type::Arrow.getId());
	}
#if !UE_BUILD_SHIPPING
	if (cmd == "start-command") {
		auto action = make_unique<CommandAction>();
		action->addStartCommand(rest);
		return std::move(action);
	}
	if (cmd == "stop-command") {
		auto action = make_unique<CommandAction>();
		action->addStopCommand(rest);
		return std::move(action);
	}
#endif /*!UI_BUILD_SHIPPING*/
	return {};
}

Unique<Objective> create(const io::Objective& def) {
	Unique<Objective> objective;

	if (def.arena) {
		objective = make_unique<ArenaBattle>(def.arena.GetValue());
	} else if (def.gauntlet) {
		objective = make_unique<Gauntlet>(def.gauntlet.GetValue());
	} else if (def.hidden) {
		objective = make_unique<Gauntlet>(def.hidden.GetValue());
	} else if (def.click) {
		if (def.click->isSequential) {
			objective = make_unique<SequentialClickPuzzle>(def.click.GetValue());
		} else {
			objective = make_unique<ClickPuzzle>(def.click.GetValue());
		}
	} else if (def.killGroup) {
		objective = make_unique<KillGroup>(def.killGroup.GetValue());
	}
	if (objective) {
		objective->narrator = createSoundData(def.sound.narrator);
		objective->objectiveTrack = createSoundData(def.sound.track);
		objective->objectiveCompletedTrack = createSoundData(def.sound.completedTrack);
		objective->setInfo(def.info);

		for (auto& meta : def.info.meta) {
			if (auto action = metaToAction(Util::toLower(meta))) {
				objective->addAction(std::move(action));
			}
		}
	}

	return objective;
}

TArray<Unique<Objective>> create(const std::vector<io::Objective>& defs) {
	TArray<Unique<Objective>> objectives;

	for (auto&& def : defs) {
		if (auto objective = create(def)) {
			objectives.Add(std::move(objective));
		}
	}
	return objectives;
}

}}
