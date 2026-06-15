// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsGameplayCueNotify_Actor.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>


ADungeonsGameplayCueNotify_Actor::ADungeonsGameplayCueNotify_Actor() {
	bAutoAttachToOwner = true;
	bAutoDestroyOnRemove = true;
}

void ADungeonsGameplayCueNotify_Actor::BeginPlay() {
	AActor::BeginPlay();
	AttachToOwnerIfNecessaryWithSockets();
}

void ADungeonsGameplayCueNotify_Actor::SetOwner(AActor* InNewOwner) {
	// Remove our old delegate
	ClearOwnerDestroyedDelegate();

	AActor::SetOwner(InNewOwner);
	if (AActor* NewOwner = GetOwner())
	{
		NewOwner->OnDestroyed.AddDynamic(this, &AGameplayCueNotify_Actor::OnOwnerDestroyed);
		DeferredAttachment = !IsAttachedTo(InNewOwner);
	}
}


int32 GameplayCueNotifyTagCheckOnRemoveDungeons = 1;
static FAutoConsoleVariableRef CVarGameplayCueNotifyActorStacking(TEXT("AbilitySystem.GameplayCueNotifyTagCheckOnRemove"), GameplayCueNotifyTagCheckOnRemoveDungeons, TEXT("Check that target no longer has tag when removing GamepalyCues"), ECVF_Default);

void ADungeonsGameplayCueNotify_Actor::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) {
	// If cvar is enabled, check that the target no longer has the matched tag before doing remove logic. This is a simple way of supporting stacking, such that if an actor has two sources giving him the same GC tag, it will not be removed when the first one is removed.
	if (GameplayCueNotifyTagCheckOnRemoveDungeons > 0 && EventType == EGameplayCueEvent::Removed) {
		if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyTarget)) {
			// MH: This check is a bit dangerous. If there is another tag applied to this target with a similar name 
			// (like GameplayCue.StatusEffect.Enchanted and GameplayCue.StatusEffect.Enchanted.Enchanter) it will treat both the same, only removing one of them,
			// the solution in my case was to rename GameplayCue.StatusEffect.Enchanted.Enchanter to GameplayCue.StatusEffect.EnchantedEnchanter which would treat them as separate tags, 
			// however, we should probably think about another check or a more safe way to do this
			if (abilitySystem->HasMatchingGameplayTag(Parameters.MatchedTagName)) {
				return;
			}
		}
	}

	if (DeferredAttachment) {
		AttachToOwnerIfNecessaryWithSockets();
		DeferredAttachment = false;
	}

	Super::HandleGameplayCue(MyTarget, EventType, Parameters);
}

bool ADungeonsGameplayCueNotify_Actor::HandlesEvent(EGameplayCueEvent::Type EventType) const {
	auto cls = GetClass();

	auto field = cls->FindFunctionByName(effects::CueFunctionNameFromEventType(EventType), EIncludeSuperFlag::IncludeSuper);

	return field->GetSuperFunction() != nullptr;
}	


void ADungeonsGameplayCueNotify_Actor::AttachToOwnerIfNecessaryWithSockets() {
	if (AActor* MyOwner = GetOwner())
	{
		if (bAutoAttachToOwner)
		{
			AttachToActor(MyOwner, FAttachmentTransformRules(LocationRule, RotationnRule, ScaleRule, false), Socket);
		}
	}
}
