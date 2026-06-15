#include "FixupLobbyChestCommandlet.h"
#include "Engine/SCS_Node.h"

#define UNIQUEID_EXCLUDE_STATIC //Hack tiem
#include "game/item/ItemType.h"
#include "game/actor/character/loot/LobbyChest.h"


void UFixupLobbyChestCommandlet::PerformAdditionalOperations(UObject* Object, bool& bSavePackage) {
	 if (ALobbyChest* cdo = Cast<ALobbyChest>(Object)) {
	 	/*
		if (cdo->StaticRewardItem != EItemType::Unset) {
			cdo->StaticRewardItemId = FSerializableItemId(game::item::type::getItemType(cdo->StaticRewardItem).getNameId());
		} else {
			cdo->StaticRewardItemId = GetItemRegistry().GetValues()[0]->getNameId();
		}
	 	*/
		bSavePackage = true;
	}
}