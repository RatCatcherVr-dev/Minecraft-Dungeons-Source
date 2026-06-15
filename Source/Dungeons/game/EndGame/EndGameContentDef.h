#pragma once
#include "EndGameContent.h"
#include "game/util/UnlockKeyUtils.h"

class UCharacterSerializeComponent;

namespace endgamecontent {

using UnlockTextProvider = std::function<FText()>;
using UnlockPredicate = std::function<bool(const UCharacterSerializeComponent*)>;

struct EndGameContentDef {
	EndGameContentDef(EEndGameContentType type);

	EEndGameContentType type() const;

	const TArray<FString>& getGrantedProgressKeys() const;
	
	TOptional<FText> getUnlockRequirementText() const;

	bool isUnlocked(const UCharacterSerializeComponent*) const;
protected:
	EEndGameContentType mType;
	UnlockPredicate mUnlockPred;
	TArray<FString> mGrantedProgressKeys;
	UnlockTextProvider mUnlockRequirementText;
};

}