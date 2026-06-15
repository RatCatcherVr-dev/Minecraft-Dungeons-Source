#include "Dungeons.h"
#include "GameplayEffectTriggerBoxComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>

UGameplayEffectTriggerBoxComponent::UGameplayEffectTriggerBoxComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(FName("PawnTrigger"));
}

void UGameplayEffectTriggerBoxComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetOwner()->HasAuthority()) {
		OnComponentBeginOverlap.AddDynamic(this, &UGameplayEffectTriggerBoxComponent::OnOverlapBegin);
		OnComponentEndOverlap.AddDynamic(this, &UGameplayEffectTriggerBoxComponent::OnOverlapEnd);
	}
}

FActiveGameplayEffectHandle applyGamePlayEffect(UAbilitySystemComponent& abilitySystem, UGameplayEffect& effectTemplate, AActor* instigator) {
	auto context = abilitySystem.MakeEffectContext();
	context.AddInstigator(instigator, instigator);
	context.AddSourceObject(instigator);
	FGameplayEffectSpec spec(&effectTemplate, context, 1);
	return abilitySystem.ApplyGameplayEffectSpecToSelf(spec);
}

void UGameplayEffectTriggerBoxComponent::OnOverlapBegin(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (auto abilitySystem = OtherActor->FindComponentByClass<UAbilitySystemComponent>()) {
		for (const auto& effectClass : Effects) {
			if (auto effectTemplate = Cast<UGameplayEffect>(effectClass->GetDefaultObject())) {
				applyGamePlayEffect(*abilitySystem, *effectTemplate, GetOwner());
			}
		}
	}
}

void UGameplayEffectTriggerBoxComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (auto abilitySystem = OtherActor->FindComponentByClass<UAbilitySystemComponent>()) {
		FGameplayEffectQuery query;
		query.EffectSource = GetOwner();
		abilitySystem->RemoveActiveEffects(query);
	}
}
