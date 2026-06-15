#include "Dungeons.h"
#include "EndGameContentDef.h"
#include "game/component/CharacterSerializeComponent.h"

namespace endgamecontent {

EndGameContentDef::EndGameContentDef(EEndGameContentType type) : mType(type) {}


EEndGameContentType EndGameContentDef::type() const {
	return mType;
}

const TArray<FString>& EndGameContentDef::getGrantedProgressKeys() const {
	return mGrantedProgressKeys;
}

bool EndGameContentDef::isUnlocked(const UCharacterSerializeComponent* serialize) const {
	return mUnlockPred ? mUnlockPred(serialize) : false;
}

TOptional<FText> EndGameContentDef::getUnlockRequirementText() const {
	return mUnlockRequirementText ? mUnlockRequirementText() : TOptional<FText>();
}

}