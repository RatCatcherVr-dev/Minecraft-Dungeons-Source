// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsGameplayCueManager.h"
#include <AbilitySystemGlobals.h>
#include "DungeonsAbilitySystemGlobals.h"
#include "effects/GameplayEffectUtil.h"
#include "GameplayCueSet.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCueNotify_Actor.h"

template<class AllocatorType>
void PullGameplayCueTagsFromSpecDuplicate(const FGameplayEffectSpec& Spec, TArray<FGameplayTag, AllocatorType>& OutArray)
{
	// Add all GameplayCue Tags from the GE into the GameplayCueTags PendingCue.list
	for (const FGameplayEffectCue& EffectCue : Spec.Def->GameplayCues)
	{
		for (const FGameplayTag& Tag : EffectCue.GameplayCueTags)
		{
			if (Tag.IsValid())
			{
				OutArray.Add(Tag);
			}
		}
	}
}


void UDungeonsGameplayCueManager::OnEngineInitComplete() {

	RuntimeGameplayCueObjectLibrary.OnLoaded = FOnGameplayCueNotifySetLoaded::CreateUObject(this, &UDungeonsGameplayCueManager::OnRutimeCueSetLoaded);
	
	Super::OnEngineInitComplete();
}

void UDungeonsGameplayCueManager::OnRutimeCueSetLoaded(TArray<FSoftObjectPath>) {
	//Our goal here is to find all gameplay cues that have a handler that accepts Executed events. This is to figure out if we should invoke executed events or not.
	for(auto entry : RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap) {
		for(int index = entry.Value; index != INDEX_NONE;) {
			FGameplayCueNotifyData& CueData =  RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[index];
	
			if(!CueData.LoadedGameplayCueClass) {
				CueData.LoadedGameplayCueClass = FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString());
				if(!CueData.LoadedGameplayCueClass) {
					ABILITY_LOG(Warning, TEXT("Failed to find cue class %s when generating Accelerator Set. Class not loaded."), *CueData.GameplayCueNotifyObj.ToString());
					continue;
				}
			}

			if (UGameplayCueNotify_Static* NonInstancedCue = Cast<UGameplayCueNotify_Static>(CueData.LoadedGameplayCueClass->ClassDefaultObject)) {
				if(NonInstancedCue->HandlesEvent(EGameplayCueEvent::Executed)) {
					OnExecuteAccelerationSet.Add(entry.Key);
					break;
				//We've found an override handler that does not accept our event, so we do not need to walk up the hierarchy.
				} else if (NonInstancedCue->IsOverride) {
					break;
				}
			} else if(AGameplayCueNotify_Actor* InstancedCue = Cast<AGameplayCueNotify_Actor>(CueData.LoadedGameplayCueClass->ClassDefaultObject)) {
				if(InstancedCue->HandlesEvent(EGameplayCueEvent::Executed)) {
					OnExecuteAccelerationSet.Add(entry.Key);
					break;
				} else if (InstancedCue->IsOverride) {
					break;
				}
			}

			//Step to parent tag
			index = CueData.ParentDataIdx;
		}
	}
}

void UDungeonsGameplayCueManager::InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent, const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) {
	if (Spec.Def->GameplayCues.Num() == 0)
	{
		// This spec doesn't have any GCs, so early out
		ABILITY_LOG(Verbose, TEXT("No GCs in this Spec, so early out: %s"), *Spec.Def->GetName());
		return;
	}

	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);

	//Mojang special sauce.
	if (context->AdditionalCues.Num()) {
		FGameplayCuePendingExecute PendingCue;
		PendingCue.PredictionKey = PredictionKey;
		PendingCue.OwningComponent = OwningComponent;

		// If we're using dynamic cues we need to use cue params to be able to send arbitrary number of cues.
		PendingCue.PayloadType = EGameplayCuePayloadType::CueParameters;

		PullGameplayCueTagsFromSpecDuplicate(Spec, PendingCue.GameplayCueTags);
		for (const auto& entry : context->AdditionalCues) {
			PendingCue.GameplayCueTags.Add(entry);
		}
		if (PendingCue.GameplayCueTags.Num() == 0)
		{
			ABILITY_LOG(Warning, TEXT("GE %s has GameplayCues but not valid GameplayCue tag."), *Spec.Def->GetName());
			return;
		}

		UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(PendingCue.CueParameters, Spec);

		if (ProcessPendingCueExecute(PendingCue))
		{
			PendingExecuteCues.Add(PendingCue);
		}

		if (GameplayCueSendContextCount == 0)
		{
			// Not in a context, flush now
			FlushPendingCues();
		}
	}
	else {
		Super::InvokeGameplayCueExecuted_FromSpec(OwningComponent, Spec, PredictionKey);
	}	
}

FVector UDungeonsGameplayCueManager::GetFullNormalFromCueParameters(const FGameplayCueParameters& SourceParams)
{
	return effects::GetPushbackVectorFromParams(SourceParams);
}

TAutoConsoleVariable<int32> CVarAllowTranslatedCues(
	TEXT("Dungeons.AbilitySystem.AllowTranslatedCues"),
	0,
	TEXT("Enable/Disable text")
	TEXT("<= 0: off.\n")
	TEXT(">= 1: on.\n"),
	ECVF_Default);


bool UDungeonsGameplayCueManager::ProcessPendingCueExecute(FGameplayCuePendingExecute& PendingCue) {
	const bool allowTranslations = CVarAllowTranslatedCues.GetValueOnGameThread() != 0;

	if(allowTranslations) {
		switch(PendingCue.PayloadType) {
		case EGameplayCuePayloadType::EffectContext:
			PendingCue.CueParameters = FGameplayCueParameters(PendingCue.CueParameters.EffectContext);
			break;
		case EGameplayCuePayloadType::FromSpec:
			PendingCue.CueParameters = FGameplayCueParameters(PendingCue.FromSpec);
		}
	}


	auto* targetActor = PendingCue.OwningComponent->GetAvatarActor();

	if(Cast<IGameplayCueInterface>(targetActor)) return true;
	
	for(auto cue : PendingCue.GameplayCueTags) {
		if(allowTranslations) TranslateGameplayCue(cue, targetActor, PendingCue.CueParameters);

		if(OnExecuteAccelerationSet.Contains(cue)) return true;
	}
	
	return false;
}

void UDungeonsGameplayCueManager::RefreshExecutionAccelerationSet() {
	OnExecuteAccelerationSet.Empty();
	OnRutimeCueSetLoaded({});
}



