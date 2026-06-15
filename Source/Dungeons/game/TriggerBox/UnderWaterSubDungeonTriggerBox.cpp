#include "UnderWaterSubDungeonTriggerBox.h"
#include "UObjectBase.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "../abilities/effects/UnderwaterImmunityGameplayEffect.h"
#include "../abilities/effects/GameplayEffectUtil.h"


void AUnderWaterSubDungeonTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (auto* Character = Cast<ABaseCharacter>(OtherActor))
	{
		UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent();
		if (AbilitySystem)
		{
			FGameplayEffectSpec Spec = effects::CreateGameplayEffectSpec<UUnderwaterImmunityGameplayEffect>(AbilitySystem);
			ActiveGameplayEffectsHandles.FindOrAdd(Character).Add(AbilitySystem->ApplyGameplayEffectSpecToSelf(Spec));
		}
	}
}

void AUnderWaterSubDungeonTriggerBox::NotifyActorEndOverlap(AActor* OtherActor)
{
	// As we disable actors collision when they teleport, they will trigger this and
	// reappear with a bubble. So we will assume that this actor is still inside the trigger
	// if this end overlap is called and the actor doesn't have collision enabled.
	// This will also work with wall mobs
	if (!OtherActor->GetActorEnableCollision()) {
		return;
	}

	Super::NotifyActorEndOverlap(OtherActor);

	if (auto* Character = Cast<ABaseCharacter>(OtherActor))
	{
		if (!Character->IsAlive() && Character->IsA<AMobCharacter>()) {
			return;
		}

		UAbilitySystemComponent* AbilitySystem = Character->GetAbilitySystemComponent();
		if (AbilitySystem && ActiveGameplayEffectsHandles.Contains(Character))
		{
			for (uint8 i = 0; i < ActiveGameplayEffectsHandles[Character].Num(); ++i)
			{
				AbilitySystem->RemoveActiveGameplayEffect(ActiveGameplayEffectsHandles[Character][i]);
			}
		}
		ActiveGameplayEffectsHandles[Character].Empty();
	}
}
