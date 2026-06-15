#include "FixupItemTypeIDCommandlet.h"
#include "Engine/SCS_Node.h"

#define UNIQUEID_EXCLUDE_STATIC //Hack tiem
#include "game/item/ItemType.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/item/StorableItem.h"


void UFixupTypeIDCommandlet::PerformAdditionalOperations(UObject* Object, bool& bSavePackage) {
	
	if (UBlueprint* SourceBlueprint = Cast<UBlueprint>(Object)) {
		if (SourceBlueprint->GeneratedClass != nullptr && SourceBlueprint->ParentClass != nullptr) {
			if (SourceBlueprint->ParentClass->IsChildOf<AItemInstance>()) {
				if(UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(*SourceBlueprint->GeneratedClass)) {
					AItemInstance* cdo = BPGC->GetDefaultObject<AItemInstance>();
				}
			} else if (SourceBlueprint->ParentClass->IsChildOf<AStorableItem>()) {
				if(UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(*SourceBlueprint->GeneratedClass)) {
					AStorableItem* cdo = BPGC->GetDefaultObject<AStorableItem>();
				}
			}
		}
	}
}