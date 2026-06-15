#include "Dungeons.h"
#include "UnrealNetwork.h"
#include "AffectorAttributeSet.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffectExtension.h>

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(DealDamageMul, UAffectorAttributeSet)

void UAffectorAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAffectorAttributeSet, DealDamageMul, COND_None, REPNOTIFY_Always);
}

DEFINE_ATTRIBUTE_FUNCTION(DealDamageMul, UAffectorAttributeSet)
