#include "GameplayEffectPushVolume.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

// ---------- GAMEPLAY EFFECTS ---------- // 
UWindPushVolumeGameplayEffect::UWindPushVolumeGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume.Wind"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.StatusEffect.PushVolume.Wind"), 0, 1);
}

UCurrentPushVolumeGameplayEffect::UCurrentPushVolumeGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::None;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume.Current"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.StatusEffect.PushVolume.Current"), 0, 1);
}

// ---------- PUSH VOLUME ---------- //
AGameplayEffectPushVolume::AGameplayEffectPushVolume() {

}

void AGameplayEffectPushVolume::AddInfluence(UPushVolumeReactiveComponent* receiverToAdd) {
	FPushVolumeInfluenceInstance newInstance(this, receiverToAdd, GetForce(), PushVolumeType);
	if (receiverToAdd->ApplyPushVolumeInfluence(newInstance)) {
		ActiveInfluences.AddUnique(newInstance);
		OnInfluenceSuccessfullyAddedToActor(receiverToAdd);
	}
}

void AGameplayEffectPushVolume::OnInfluenceSuccessfullyAddedToActor(UPushVolumeReactiveComponent* reciever) {
	// We added influences to this actor, so add a gameplay effect
	if (UAbilitySystemComponent* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(reciever->GetOwner())) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, EffectToApply);
		effects::GetDungeonsContextFromSpec(spec)->Normal = reciever->GetCumulativePushDirection();
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void AGameplayEffectPushVolume::OnInfluenceSuccessfullyRemovedFromActor(UPushVolumeReactiveComponent* reciever) {
	// We removed influences from this actor, so remove the gameplay effect
	if (UAbilitySystemComponent* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(reciever->GetOwner())) {
		abilitySystem->RemoveActiveGameplayEffectBySourceEffect(EffectToApply, abilitySystem, 1);
	}

}
