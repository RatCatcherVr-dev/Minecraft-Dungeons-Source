#include "Dungeons.h"
#include "UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "StatusAttributeSet.h"

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(PositiveStatusDurationMagnitude, UStatusAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(NegativeStatusDurationMagnitude, UStatusAttributeSet)

void UStatusAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStatusAttributeSet, PositiveStatusDurationMagnitude, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStatusAttributeSet, NegativeStatusDurationMagnitude, COND_None, REPNOTIFY_Always);
}

DEFINE_ATTRIBUTE_FUNCTION(PositiveStatusDurationMagnitude, UStatusAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(NegativeStatusDurationMagnitude, UStatusAttributeSet)