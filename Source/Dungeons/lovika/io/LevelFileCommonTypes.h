#pragma once

#include "lovika/QuadrantAngle.h"
#include "game/Difficulty/Difficulty.h"
#include <string>
#include <vector>

struct CaseInsensitiveId {
	CaseInsensitiveId() {}
	CaseInsensitiveId(std::string casedId);

	bool operator==(const CaseInsensitiveId&) const;
	bool operator!=(const CaseInsensitiveId&) const;
	bool operator==(const std::string&) const;
	bool operator!=(const std::string&) const;
	bool operator<(const CaseInsensitiveId&) const;
	bool equalsAlreadyLowerCase(const std::string&) const;

	std::string id;
	std::string lowerId;
};

struct WeightedId: public CaseInsensitiveId {
	WeightedId(float weight = 1.0f);
	WeightedId(CaseInsensitiveId, float weight = 1.0f);

	float weight;

	float getWeight() const;
};


struct Rotations {
	Rotations();
	explicit Rotations(std::vector<QuadrantAngle>);
	bool has(QuadrantAngle) const;
	const std::vector<QuadrantAngle>& get() const;
private:
	bool mHasAllRotations;
	std::vector<QuadrantAngle> mRotations;
};


struct DifficultyRange {
	DifficultyRange();
	DifficultyRange(std::vector<EGameDifficulty>);
	bool has(EGameDifficulty) const;
	const std::vector<EGameDifficulty>& get() const;
private:
	bool mHasAllValues;
	std::vector<EGameDifficulty> mValues;
};


UENUM()
enum class ECharacterLoadoutType : uint8 {
	DefaultLoadout,
	TutorialLoadout,
};
ENUM_NAME(ECharacterLoadoutType);


//
// We've removed the namespace from the above types that have lesser risk
// of clashing with similar types from other places in the solution.
//
// Example: This simple 'Interval' struct is too bare-bone to be THE
//          type called 'Interval'. It's fine for io::Interval though.
//
namespace io {

struct Interval {
	int min, max;
};

struct ObjectGroupDef {
	ObjectGroupDef() = default;
	ObjectGroupDef(const std::string& path);

	CaseInsensitiveId name;
	CaseInsensitiveId path;
};

}
