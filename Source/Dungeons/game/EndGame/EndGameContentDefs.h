#pragma once

#include "EndGameContent.h"
#include "EndGameContentDef.h"
#include "MutableEndGameContentDef.h"
#include "game/defs/DefsContainer.h"
#include <Kismet/BlueprintFunctionLibrary.h>

class UMissionThemeLibrary;

namespace endgamecontent {
	using EndGameContentDefs = DefsContainer<MutableEndGameContentDef, EndGameContentDef, EEndGameContentType>;
	const EndGameContentDefs& defs();
	TArray<EEndGameContentType> allTypes();
	TArray<EEndGameContentType> unannouncedTypes(const TArray<EEndGameContentType>& announced);
	TArray<EEndGameContentType> unlockedUnannouncedTypes(const UCharacterSerializeComponent*);
	bool isUnlocked(const UCharacterSerializeComponent*, EEndGameContentType);
	TArray<FString> getAllUnlockedProgressKeys(const UCharacterSerializeComponent*);
}
