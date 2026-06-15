#pragma once
#include "SelectPlayerCharacter.h"
#include "SelectOtherPlayerCharacter.generated.h"

class APlayerCharacter;

UCLASS(BlueprintType)
class DUNGEONS_API USelectOtherPlayerCharacter : public USelectPlayerCharacter {
	GENERATED_BODY()
protected:
	bool IsPlayerCharacterSelectable(APlayerCharacter* playerCharacter) const override;
};

