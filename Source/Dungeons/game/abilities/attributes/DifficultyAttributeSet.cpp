#include "Dungeons.h"
#include "UnrealNetwork.h"
#include "DifficultyAttributeSet.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffectExtension.h>

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(DifficultyDealDamageMultiplier, UDifficultyAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(DifficultyPerformHealingMultiplier, UDifficultyAttributeSet)

void UDifficultyAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDifficultyAttributeSet, DifficultyDealDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDifficultyAttributeSet, DifficultyPerformHealingMultiplier, COND_None, REPNOTIFY_Always);

}
DEFINE_ATTRIBUTE_FUNCTION(DifficultyDealDamageMultiplier, UDifficultyAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(DifficultyPerformHealingMultiplier, UDifficultyAttributeSet)
