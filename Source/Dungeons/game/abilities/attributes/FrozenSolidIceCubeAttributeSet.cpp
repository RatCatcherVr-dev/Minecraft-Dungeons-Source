#include "Dungeons.h"
#include "UnrealNetwork.h"
#include "FrozenSolidIceCubeAttributeSet.h"
#include "AbilitySystemComponent.h"

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(OwnerInteractionMagnitude, UFrozenSolidIceCubeAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(OtherInteractionMagnitude, UFrozenSolidIceCubeAttributeSet)

void UFrozenSolidIceCubeAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFrozenSolidIceCubeAttributeSet, OwnerInteractionMagnitude, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFrozenSolidIceCubeAttributeSet, OtherInteractionMagnitude, COND_None, REPNOTIFY_Always);
}

DEFINE_ATTRIBUTE_FUNCTION(OwnerInteractionMagnitude, UFrozenSolidIceCubeAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(OtherInteractionMagnitude, UFrozenSolidIceCubeAttributeSet)
