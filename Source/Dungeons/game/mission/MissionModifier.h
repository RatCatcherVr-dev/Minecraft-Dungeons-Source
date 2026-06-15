#pragma once

struct MissionDef;
enum class ELevelNames : uint8;

namespace missions {

struct MutableMissionDef;

namespace overrides {

struct Modify {
	Modify(bool baseOnCurrent, std::function<void(MutableMissionDef&)>);

	void operator()(ELevelNames) const;
	void operator()(const MissionDef*) const;
	std::function<void(MutableMissionDef&)> f;
private:
	void update(ELevelNames) const;
	bool current;
};

}}
