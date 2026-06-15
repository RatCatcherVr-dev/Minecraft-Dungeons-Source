#include "Dungeons.h"
#include "DamageFilter.h"

bool FDamageMultiplier::operator ==(const FGameplayTag& tag) const { 
	return tag.MatchesTag(Tag);
	//return Tag.MatchesTag(tag); 
}

namespace damageFilter {
	float getMultiplier(const FDamageFilter& damageFilter, const FGameplayTagContainer& damageTypes) {
		float multiplier = 1.f;
		
		for (int i = 0; i < damageTypes.Num(); ++i) {
			if (const FDamageMultiplier* entry = damageFilter.Multipliers.FindByKey(damageTypes.GetByIndex(i))) {
				multiplier *= entry->Multiplier;
			}
		}

		return multiplier;
	}
}
