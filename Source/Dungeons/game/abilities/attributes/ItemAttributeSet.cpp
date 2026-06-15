// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "UnrealNetwork.h"
#include <AbilitySystemComponent.h>
#include "ItemAttributeSet.h"

DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ItemCooldownMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ItemDamageMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(SoulGathering, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(SoulGatheringMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(SoulSpawnMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(ItemLifeStealAmount, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(PotionCooldownMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(Souls, UItemAttributeSet)
DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(MaxSouls, UItemAttributeSet)

void UItemAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, ItemCooldownMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, ItemDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, SoulGathering, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, SoulGatheringMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, SoulSpawnMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, ItemLifeStealAmount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, PotionCooldownMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, Souls, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UItemAttributeSet, MaxSouls, COND_None, REPNOTIFY_Always);
}

void UItemAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	if (Attribute == SoulsAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, MaxSouls);
	}
}

void UItemAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const {
	if (Attribute == SoulsAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, MaxSouls);
	}
}

void UItemAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) {
	if (Data.EvaluatedData.Attribute == SoulsAttribute()) {
		OnAttemptChangeSouls.ExecuteIfBound(Data);
	}
}


DEFINE_ATTRIBUTE_FUNCTION(ItemCooldownMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ItemDamageMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(SoulGathering, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(SoulGatheringMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(SoulSpawnMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(ItemLifeStealAmount, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(PotionCooldownMultiplier, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(Souls, UItemAttributeSet)
DEFINE_ATTRIBUTE_FUNCTION(MaxSouls, UItemAttributeSet)

