#pragma once

#include "AlphaGeneratorTypes.h"

namespace generator { namespace alpha {

void generateDeadEnds(GenState&, int tileIndex, const std::vector<MetaTile>& deadEndCandidates);
void generateDeadEnds(GenState&);

class DeadEndsPerLevel {
public:
	DeadEndsPerLevel(const Env&);
	const MetaTileVector& getOrCreate(const io::DungeonLevelInfo&);
private:
	const Env& mEnv;
	TMap<FString, MetaTileVector> mLevelDeadEnds;
};

}}
