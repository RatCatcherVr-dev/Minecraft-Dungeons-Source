#include "EndlessStruggleTier.h"

class UMissionProgressComponent;

namespace endlesstruggle { namespace tier {	
	const TArray<const EndlessStruggleTier*>& getAllLowestToHighest();
	const TArray<const EndlessStruggleTier*>& getAllHighestToLowest();
	TArray<const EndlessStruggleTier*> getAllUnlocked(const UMissionProgressComponent*);
	const EndlessStruggleTier* getHighestUnlocked(const UMissionProgressComponent*);
	const EndlessStruggleTier* getLowestLocked(const UMissionProgressComponent*);
	const EndlessStruggleTier* getUnlockFor(FEndlessStruggle);
	const EndlessStruggleTier* getTierFromStat(EProgressStat);
}}