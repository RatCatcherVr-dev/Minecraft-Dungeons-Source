


#include "DungeonsGameplayCueNotify_Static.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

bool UDungeonsGameplayCueNotify_Static::HandlesEvent(EGameplayCueEvent::Type EventType) const {
	auto cls = GetClass();
	
	auto field = cls->FindFunctionByName(effects::CueFunctionNameFromEventType(EventType), EIncludeSuperFlag::IncludeSuper);

	return field->GetSuperFunction() != nullptr;
}
