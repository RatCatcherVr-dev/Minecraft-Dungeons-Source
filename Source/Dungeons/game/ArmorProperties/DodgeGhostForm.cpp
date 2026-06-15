#include "DodgeGhostForm.h"
#include "game/actor/character/BaseCharacter.h"

UGhostDodgeGameplayEffect::UGhostDodgeGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//Remove parent tag and add our derived tag
	InheritableOwnedTagsContainer.RemoveTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.Dodge"));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.Dodge"));

	//Get rid of parent underived tags.
	GameplayCues.Empty();

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Ghost.Dodge"), 0, 1);
}

UDodgeGhostForm::UDodgeGhostForm() {
	TypeID = EArmorPropertyID::DodgeGhostForm;
	Effect = UGhostDodgeGameplayEffect::StaticClass();
}

void UDodgeGhostForm::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key)
{
	auto abilitySystem = GetOwnerCharacter()->GetAbilitySystemComponent();
	abilitySystem->ApplyGameplayEffectSpecToSelf(FGameplayEffectSpec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), 1), key);

	if (GetOwnerRole() == ENetRole::ROLE_Authority && removeHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(removeHandle);
	}
}

void UDodgeGhostForm::OnDodgeRollEnd(FPredictionKey key)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority) {
		GetWorld()->GetTimerManager().SetTimer(removeHandle, FTimerDelegate::CreateUObject(this, &UDodgeGhostForm::RemoveGameplayEffect), PostDodgeDuration, false);
	}
}

void UDodgeGhostForm::RemoveGameplayEffect() {
	GetOwnerCharacter()->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk.Dodge")));
}