#pragma once

#include "UIHintDefs.generated.h"


// note: UI Hint events are accounted for in character.json (savefile) and only showed once!
// do not map events, just add the metadata straight into the FUIHint struct list
UENUM(BlueprintType)
enum class EUIHintTriggerAction : uint8
{
	Unset,
	PickupItem,
	//ItemEquipped,
	ItemUsed,
	EnterRegion,
	NewObjective,
	Move,

};
ENUM_NAME(EUIHintTriggerAction);

UENUM(BlueprintType)
enum class EUIHintClose : uint8 {
	Unset,
	ItemUsed,
	MeleeAttack,
	RangeAttack,
	EnterRegion,
	NewObjective,
	PickupItem,
	Move,

};
ENUM_NAME(EUIHintClose);

UENUM(BlueprintType)
enum class EUIHintConcept : uint8 {
	Unset,
	NavigationMarker,	
};
ENUM_NAME(EUIHintConcept);