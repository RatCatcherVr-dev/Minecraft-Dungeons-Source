#include "FixupArrowCommandlet.h"
#include "Engine/SCS_Node.h"

#define UNIQUEID_EXCLUDE_STATIC //Hack tiem
#include "game/item/ItemType.h"
#include "Components/SphereComponent.h"
#include "game/actor/item/Arrow.h"


void UFixupArrowCommandlet::PerformAdditionalOperations(UObject* Object, bool& bSavePackage) {
	/*
	if (UBlueprint* SourceBlueprint = Cast<UBlueprint>(Object)) {
		if (SourceBlueprint->GeneratedClass != nullptr && SourceBlueprint->ParentClass != nullptr) {
			if (SourceBlueprint->ParentClass->IsChildOf<AArrow>()) {
				if (UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(*SourceBlueprint->GeneratedClass)) {
					AArrow* cdo = BPGC->GetDefaultObject<AArrow>();
					if(cdo->ConsumesStorableItemType != EItemType::Arrow) {
						cdo->ConsumesStorableItemId = game::item::type::getItemType(cdo->ConsumesStorableItemType).getNameId();
						bSavePackage = true;
					}
				}
			}
		}
	}
	*/
}