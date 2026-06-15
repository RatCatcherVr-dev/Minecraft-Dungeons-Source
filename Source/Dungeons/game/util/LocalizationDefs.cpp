
#include "Dungeons.h"
#include "LocalizationDefs.h"

#include "Engine/Classes/Kismet/KismetInternationalizationLibrary.h"

#include <map>

namespace dungeonsloc {
	static std::map<FString, FText> s_textCollection  {
		{"action_revive", NSLOCTEXT("PlayerCharacter", "action_revive", "Revive") }
	};

	FText GetText(const FString& id) {
		auto it = s_textCollection.find(id);
		ensureMsgf(it != s_textCollection.end(), TEXT("No such text defined '%s', need to add to Dungeons text collection"), *id);

		if (it != s_textCollection.end()) {
			return it->second;
		}

		return FText::FromString("LocalizationDefs: Missing Text");
	}


}
