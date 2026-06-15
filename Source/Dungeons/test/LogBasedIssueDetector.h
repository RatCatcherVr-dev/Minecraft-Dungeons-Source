#pragma once

#include "CommonTypes.h"
#include <OutputDevice.h>
#include <Logging/LogVerbosity.h>

namespace logissues {

struct State {
	FName category;
	FString text;
	ELogVerbosity::Type verbosity;

	FOutputDevice* log;
};

using Predicate = ::Pred<const State&>;
using Action = std::function<void(const State&)>;

struct Detector {
	Detector(Predicate, Action);
	Detector(Predicate, std::vector<Action>);

	void eval(const State&) const;
private:
	std::function<bool(const State&)> mPred;
	std::vector<Action> mActions;
};

class Detectors: public FOutputDevice {
public:
	~Detectors();

	void add(Detector);
	void addAll(std::vector<Detector>);

	void Serialize(const TCHAR*, ELogVerbosity::Type, const FName& category) override;
private:
	std::vector<Detector> mDetectors;
};

void registerDetectors(Detectors&);
void unregisterDetectors(Detectors&);

//
// Helper predicates and detectors
//
namespace predicates {

Predicate TextContains(FString);
Predicate Category(FName);
Predicate Category(const struct FLogCategoryBase&);

}

namespace actions {

      Action  LogDefault(TOptional<ELogVerbosity::Type>, FString);
      Action  EnsureOnce(FString = {});
      Action  EnsureAlways(FString = {});
      Action  Check(FString = {});

}

namespace detectors {

const Detector& ClientAndServerLevelsDiffer();

std::vector<Detector> createDefault();

}

}
