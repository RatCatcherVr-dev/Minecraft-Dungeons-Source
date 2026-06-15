#include "Dungeons.h"
#include "SelectOtherPlayerCharacter.h"

bool USelectOtherPlayerCharacter::IsPlayerCharacterSelectable(APlayerCharacter* playerCharacter) const {
	return &GetSession().GetShopper() != playerCharacter;
}


