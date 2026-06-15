#include "Dungeons.h"
#include "ImpactFilter.h"

namespace impactFilter {
	bool shouldImpact(const FImpactFilter& impactFilter, const FGameplayTag& candidate) {
		if (impactFilter.Entries.Num() == 0) {
			return true;
		}
		
		for (const auto& entry : impactFilter.Entries) {
			if (candidate.MatchesTag(entry.Tag)) {
				return entry.Allow;
			}
		}

		return false;
	}
}
