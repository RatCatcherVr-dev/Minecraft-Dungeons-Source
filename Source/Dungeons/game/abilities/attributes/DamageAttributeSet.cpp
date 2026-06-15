// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "DamageAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UnrealNetwork.h"

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(DamageMultiplier, UDamageAttributeSet)

void UDamageAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UDamageAttributeSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
}

DEFINE_ATTRIBUTE_FUNCTION(DamageMultiplier, UDamageAttributeSet)