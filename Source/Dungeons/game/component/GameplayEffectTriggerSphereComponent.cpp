#include "Dungeons.h"
#include "GameplayEffectTriggerSphereComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include <AbilitySystemComponent.h>

UGameplayEffectTriggerSphereComponent::UGameplayEffectTriggerSphereComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UGameplayEffectTriggerSphereComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetOwner()->HasAuthority()) {
		OnComponentBeginOverlap.AddDynamic(this, &UGameplayEffectTriggerSphereComponent::OnOverlapBegin);
		OnComponentEndOverlap.AddDynamic(this, &UGameplayEffectTriggerSphereComponent::OnOverlapEnd);
	}
}

FActiveGameplayEffectHandle UGameplayEffectTriggerSphereComponent::ApplyGameplayEffect(UAbilitySystemComponent& abilitySystem, UGameplayEffect& effectTemplate, AActor* instigator) {
	auto context = abilitySystem.MakeEffectContext();
	context.AddInstigator(instigator, instigator);
	context.AddSourceObject(instigator);
	FGameplayEffectSpec spec(&effectTemplate, context, 1);
	FActiveGameplayEffectHandle newHandler = abilitySystem.ApplyGameplayEffectSpecToSelf(spec);
	ActiveGameplayEffectsHandles.FindOrAdd(abilitySystem.GetOwner()).Add(newHandler);
	return newHandler;
}

/** Helper function since we can't have default/optional values for FModifierQualifier in K2 function */
FActiveGameplayEffectHandle UGameplayEffectTriggerSphereComponent::BP_ApplyGameplayEffectToActor(UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass) {
	UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	return ApplyGameplayEffect(*abilitySystem, *GameplayEffect, GetOwner());
}

void UGameplayEffectTriggerSphereComponent::RemoveGameplayEffect(UAbilitySystemComponent& abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* instigator) {
	UAbilitySystemComponent* InstigatorAbilitySystemComponent = NULL;
	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner())) {
		InstigatorAbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	}

	abilitySystem.RemoveActiveGameplayEffectBySourceEffect(GameplayEffectClass, InstigatorAbilitySystemComponent, 1);
}

/** Helper function since we can't have default/optional values for FModifierQualifier in K2 function */
void UGameplayEffectTriggerSphereComponent::BP_RemoveGameplayEffectFromActor(UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass) {
	return RemoveGameplayEffect(*abilitySystem, GameplayEffectClass, GetOwner());
}

void UGameplayEffectTriggerSphereComponent::OnOverlapBegin(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (auto abilitySystem = OtherActor->FindComponentByClass<UAbilitySystemComponent>()) {
		for (const auto& effectClass : Effects) {
			if (auto effectTemplate = Cast<UGameplayEffect>(effectClass->GetDefaultObject())) {
				ApplyGameplayEffect(*abilitySystem, *effectTemplate, GetOwner());
			}
		}
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor)) {
			for (const auto& effectClass : PlayerOnlyEffects) {
				if (auto effectTemplate = Cast<UGameplayEffect>(effectClass->GetDefaultObject())) {
					ApplyGameplayEffect(*abilitySystem, *effectTemplate, GetOwner());
				}
			}
		}
		if (AMobCharacter* mob = Cast<AMobCharacter>(OtherActor)) {
			TArray<APlayerCharacter*> players = InstanceTracker<APlayerCharacter>::GetList(GetWorld());
			if (mob->GetMaster() && players.Contains(mob->GetMaster())) {
				return;
			}
			for (const auto& effectClass : MobOnlyEffects) {
				if (auto effectTemplate = Cast<UGameplayEffect>(effectClass->GetDefaultObject())) {
					ApplyGameplayEffect(*abilitySystem, *effectTemplate, GetOwner());
				}
			}
		}
	}
}

void UGameplayEffectTriggerSphereComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (auto abilitySystem = OtherActor->FindComponentByClass<UAbilitySystemComponent>()) {
		if (ActiveGameplayEffectsHandles.Contains(OtherActor)) {
			for (uint8 i = 0; i < ActiveGameplayEffectsHandles[OtherActor].Num(); ++i)
			{
				abilitySystem->RemoveActiveGameplayEffect(ActiveGameplayEffectsHandles[OtherActor][i]);
			}
			ActiveGameplayEffectsHandles[OtherActor].Empty();
		}
	}
}