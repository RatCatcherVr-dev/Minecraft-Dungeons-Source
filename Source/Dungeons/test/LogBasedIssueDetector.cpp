#include "Dungeons.h"
#include "LogBasedIssueDetector.h"
#include <CoreGlobals.h>
#include <LogCategory.h>
#include <LogMacros.h>

DEFINE_LOG_CATEGORY_STATIC(LogIssueDetector, Display, Display);

namespace logissues {

//
// Detector (a predicate + list of actions)
//
Detector::Detector(Predicate pred, Action action)
	: Detector(std::move(pred), std::vector<Action>{ std::move(action) }) {
}

Detector::Detector(Predicate pred, std::vector<Action> actions)
	: mPred(std::move(pred))
	, mActions(std::move(actions)) {
}

void Detector::eval(const State& state) const {
	if (mPred(state)) {
		for (auto& action : mActions) {
			action(state);
		}
	}
}

Detectors::~Detectors() {
	unregisterDetectors(*this);
}

//
// Detectors: holding a bunch of detector objects
//
void Detectors::add(Detector detector) {
	mDetectors.push_back(std::move(detector));
}

void Detectors::addAll(std::vector<Detector> detectors) {
	mDetectors.insert(mDetectors.end(), std::make_move_iterator(detectors.begin()), std::make_move_iterator(detectors.end()));
}

void Detectors::Serialize(const TCHAR* text, ELogVerbosity::Type verbosity, const FName& category) {
#if NO_LOGGING
	return;
#else
	if (category == LogIssueDetector.GetCategoryName()) {
		return;
	}
	const State state {
		category,
		text,
		verbosity,
		GLog
	};
	for (const auto& detector : mDetectors) {
		detector.eval(state);
	}
#endif
}

//
// Utility functions
//
void registerDetectors(Detectors& logDetectors) {
	GLog->AddOutputDevice((FOutputDevice*)&logDetectors);
}

void unregisterDetectors(Detectors& logDetectors) {
	GLog->RemoveOutputDevice((FOutputDevice*)&logDetectors);
}


//
// Predicates
//
namespace predicates {

Predicate TextContains(FString text) {
	return [text = std::move(text)](State state) {
		return state.text.Contains(text);
	};
}

Predicate Category(FName category) {
	return [category = std::move(category)](State state) {
		return state.category == category;
	};
}

Predicate Category(const FLogCategoryBase& category) {
	return Category(category.GetCategoryName());
}

}

//
// Actions
//
namespace actions {

Action LogDefault(TOptional<ELogVerbosity::Type> type, FString description) {
#if NO_LOGGING
	return [](const State& state) {};
#else
	return [type, description = std::move(description)](const State& state) {
		state.log->Log(
			LogIssueDetector.GetCategoryName(),
			type.Get(state.verbosity),
			FString::Format(TEXT("LogBasedIssueDetector found a warning '{0}' based on log message '{1}'"), {
				description,
				state.text
			})
		);
	};
#endif
}

Action EnsureOnce(FString text) {
	return [text = std::move(text)](const State&) {
		ensureMsgf(false, TEXT("%s"), *text);
	};
}

Action EnsureAlways(FString text) {
	return[text = std::move(text)](const State&) {
		ensureAlwaysMsgf(false, TEXT("%s"), *text);
	};
}

Action Check(FString text) {
	return[text = std::move(text)](const State&) {
		checkf(false, TEXT("%s"), *text);
	};
}

}


//
// Implemented detectors
//
namespace detectors {

const Detector& ClientAndServerLevelsDiffer() {
	static const auto detector = Detector(
		predicates::TextContains("ServerUpdateLevelVisibility() ignored non-existant package"),
		actions::LogDefault(ELogVerbosity::Error, "Client and Server actors differ! (if level related: different level, seed or using non-seeded random values in generation)")
	);
	return detector;
}

const Detector& ClassOrOuterMisaligned() {
	static const auto detector = Detector(
		predicates::TextContains("Object flags are invalid or either Class or Outer is misaligned"),
		actions::LogDefault(ELogVerbosity::Error, "Object might not be stably named")
	);
	return detector;
}

	
std::vector<Detector> createDefault() {
	using namespace detectors;
	return {
		ClientAndServerLevelsDiffer(),
		ClassOrOuterMisaligned()
	};
}

}

}
