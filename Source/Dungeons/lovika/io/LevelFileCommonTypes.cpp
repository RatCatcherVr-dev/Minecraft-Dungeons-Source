#include "Dungeons.h"
#include "LevelFileCommonTypes.h"
#include "util/Algo.h"

//
// CaseInsensitiveId
//
CaseInsensitiveId::CaseInsensitiveId(std::string casedId)
	: id(std::move(casedId))
	, lowerId(Util::toLower(id)) {
}

bool CaseInsensitiveId::equalsAlreadyLowerCase(const std::string& rhs) const {
	return lowerId == rhs;
}

bool CaseInsensitiveId::operator==(const CaseInsensitiveId& rhs) const {
	return lowerId == rhs.lowerId;
}

bool CaseInsensitiveId::operator==(const std::string& rhs) const {
	return lowerId == Util::toLower(rhs);
}

bool CaseInsensitiveId::operator!=(const CaseInsensitiveId& rhs) const {
	return lowerId != rhs.lowerId;
}

bool CaseInsensitiveId::operator!=(const std::string& rhs) const {
	return lowerId != Util::toLower(rhs);
}

bool CaseInsensitiveId::operator<(const CaseInsensitiveId& rhs) const {
	return lowerId < rhs.lowerId;
}

//
//
// WeightedId
//
WeightedId::WeightedId(CaseInsensitiveId id, float weight /*= 1.0f*/)
	: CaseInsensitiveId(std::move(id))
	, weight(weight) {
}

WeightedId::WeightedId(float weight /*= 1.0f*/)
	: CaseInsensitiveId("")
	, weight(weight) {
}

float WeightedId::getWeight() const {
	return weight;
}


template <typename T>
static bool checkHasAllValues(T first, T last, const std::vector<T>& values) {
	const int firstIndex = static_cast<int>(first);
	const int size = static_cast<int>(last) - firstIndex + 1;

	std::vector<uint8_t> remaining(size, 1);

	int count = 0;
	for (auto val : values) {
		const int i = static_cast<int>(val) - firstIndex;
		count += remaining[i];
		remaining[i] = 0;
	}
	return count == size;
}
//
// Rotations
//
static bool checkHasAllRotations(const std::vector<QuadrantAngle>& rotations) {
	return checkHasAllValues(QuadrantAngle::D0, QuadrantAngle::D270, rotations);
}

Rotations::Rotations()
	: mHasAllRotations(true) {
}

Rotations::Rotations(std::vector<QuadrantAngle> rotations)
	: mRotations(rotations)
	, mHasAllRotations(checkHasAllRotations(rotations)) {
}

const std::vector<QuadrantAngle>& Rotations::get() const {
	if (mHasAllRotations) {
		static const std::vector<QuadrantAngle> all(std::begin(quadrantAngles()), std::end(quadrantAngles()));
		return all;
	}
	return mRotations;
}

bool Rotations::has(QuadrantAngle rotation) const {
	return mHasAllRotations || algo::contains(mRotations, rotation);
}


//
// DifficultyRange
//
DifficultyRange::DifficultyRange()
	: mHasAllValues(true) {
}

DifficultyRange::DifficultyRange(std::vector<EGameDifficulty> difficulties)
	: mValues(difficulties)
	, mHasAllValues(checkHasAllValues(difficultyquery::First, difficultyquery::Last, difficulties)) {
}

bool DifficultyRange::has(EGameDifficulty difficulty) const {
	return mHasAllValues || algo::contains(mValues, difficulty);
}

const std::vector<EGameDifficulty>& DifficultyRange::get() const {
	if (mHasAllValues) {
		return difficultyquery::AllDifficulties;
	}
	return mValues;
}


namespace io {

ObjectGroupDef::ObjectGroupDef(const std::string& p)
	: path(p)
{
	const auto sep = p.find('/');
	name = sep != std::string::npos ? p.substr(0, sep) : p;
}

}
