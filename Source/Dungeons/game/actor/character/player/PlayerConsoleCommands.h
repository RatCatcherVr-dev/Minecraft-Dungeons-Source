#pragma once
#include "game/item/ItemType.h"

namespace merchantconsolehandlers {

	template <EProgressStat P>
	void DoIncrementProgressStat(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {		
		auto count = ArgAsInt(args, 0).Get(1);
		for (auto character : GetAllPlayerCharacters(world, args)) {
			if (character->IsLocallyControlled()) {
				character->GetCharacterSerializeComponent()->IncrementProgressStat(P, count);
			}
		}
	}	
	void DoIncrementProgressStats(const TArray<FString>& args, UWorld* world, FOutputDevice& out, TArray<EProgressStat> stats) {		
		auto count = ArgAsInt(args, 0).Get(1);
		for (auto character : GetAllPlayerCharacters(world, args)) {
			if (character->IsLocallyControlled()) {
				for (auto stat : stats) {
					character->GetCharacterSerializeComponent()->IncrementProgressStat(stat, count);
				}
			}
		}		
	}	
}