#include "Dungeons.h"
#include "ObjectivesSystemUtil.h"
#include "Objective.h"
#include "ObjectiveUtil.h"
#include "ObjectiveValidationType.h"
#include "game/level/GameTile.h"
#include "util/Algo.hpp"
#include "util/StringUtil.h"
#include "util/UELogHelpers.h"
#include <CoreGlobals.h>

namespace game { namespace objective { namespace util {

bool validate(const std::string& prefix, const Objective& objective, ValidationType type, FOutputDevice* log /*= nullptr*/) {
	auto validation = objective.validate(type);
	// Base objective validation. @todo? move this to (non-virtual) Objective::doValidate
	// which does the base objective validation and calls virtual validate()
	if (!objective.info().requiredRegions.empty()) {
		const auto missingRegions = algo::copy_if(objective.info().requiredRegions, RETLAMBDA(objective.targetFinder().get(it).empty()));
		if (!missingRegions.empty()) {
			validation.error("Couldn't find required regions: " + Util::join(missingRegions));
		}
	}

	if (validation.isSuccessNoWarnings()) {
		return true;
	}
#if NO_LOGGING == 0
	const auto prefixDescription = FString::Printf(TEXT("Objectives.%sValidation %s %s%s"),
		UTF8_TO_TCHAR((type == ValidationType::Init ? "Init" : "Start")),
		UTF8_TO_TCHAR((validation.isSuccess() ? "warned.": "failed. Skipping")),
		UTF8_TO_TCHAR((objective.info().isOptional? "OPTIONAL " : "")),
		*stringutil::toFString(prefix));

	const auto verbosity = [&] {
		if (objective.info().isOptional) {
			return ELogVerbosity::Display;
		}
		return validation.isSuccess() ? ELogVerbosity::Warning : ELogVerbosity::Error;
	}();

	GetLogOutputOrDefault(log).CategorizedLogf(LogObjective.GetCategoryName(), verbosity, TEXT("%s '%s': %s (#%d/%d in list) because: %s"),
		*prefixDescription,
		*identifiableName(objective.info()),
		*identifiableDescription(objective.info()),
		objective.info().objectiveIndex + 1,
		objective.info().objectiveCount,
		UTF8_TO_TCHAR(validation.summary().c_str())
	);
#endif
	return validation.isSuccess();
}

std::vector<TilePtr> getTiles(const std::vector<TilePtr>& allTiles, io::ObjectiveConsiderTilesType considerType, const Tile* base/*= nullptr*/) {
	if (considerType == io::ObjectiveConsiderTilesType::Tile) {
		ensure(base);
		return { base };
	}
	if (considerType == io::ObjectiveConsiderTilesType::Stretch) {
		ensure(base);
		const auto stretch = base->progress().stretches().index();
		return algo::copy_if(allTiles, RETLAMBDA(!it->progress().stretches().index() == stretch));
	}
	if (considerType == io::ObjectiveConsiderTilesType::Main) {
		return algo::copy_if(allTiles, RETLAMBDA(!it->progress().isOnStrayPath()));
	}
	return allTiles;
}

}}}

FOutputDevice& thisOrGlobalLog(FOutputDevice* log) {
	return log ? *log : *GLog;
}
