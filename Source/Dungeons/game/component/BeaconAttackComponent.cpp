#include "Dungeons.h"
#include "BeaconAttackComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "AbilitySystemComponent.h"
#include "game/actor/DynamicBeam.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "AbilitySystemGlobals.h"

UBeaconAttackComponent::UBeaconAttackComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bReplicates = true;
}

void UBeaconAttackComponent::BeginPlay() {
	Super::BeginPlay();
	
	for (auto* component : GetOwner()->GetComponentsByTag(UChildActorComponent::StaticClass(), "beam")) {
		beamChildren.Add(Cast<UChildActorComponent>(component));
	}
}

void UBeaconAttackComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) {
	Super::TickComponent(deltaTime, tickType, thisTickFunction);
	
	const auto now = GetWorld()->GetTimeSeconds();

	if (now - startTime < delay) {
		return;
	}

	const auto timeDelta = now - (startTime + delay);
	if (timeDelta > duration) {
		SetComponentTickEnabled(false);
		
		for (auto* beamChild : beamChildren) {
			beamChild->DestroyChildActor();
		}
		
		BeaconAttackEnded.Broadcast();
		return;
	}

	const auto yaw = SweepOnce(timeDelta / duration) * sweepAngleSpan;

	for (auto* beamChild : beamChildren) {
		beamChild->SetWorldRotation(FRotator { 0.f, yaw, 0.f });
	}
}

void UBeaconAttackComponent::MulticastStart_Implementation() {
	Start();
}

void UBeaconAttackComponent::Start() {
	SetComponentTickEnabled(true);

	if (auto owner = Cast<ABaseCharacter>(GetOwner())) {
		owner->RemoveInvisibility();
	}

	startTime = GetWorld()->GetTimeSeconds();

	for (auto* beamChild : beamChildren) {
		beamChild->SetWorldRotation(FRotator::ZeroRotator);
		beamChild->SetChildActorClass(beamClass);		
	}	

	BeaconAttackStarted.Broadcast();	
}

float UBeaconAttackComponent::SweepOnce(float fraction) {
	return fraction < 0.f ? 0.f 
		: fraction > 1.f ? 1.f 
		: .5f * (1.f - FMath::Cos(fraction * PI));
}

float UBeaconAttackComponent::SweepTwice(float fraction) {
	return FMath::Pow(FMath::Max(0.f, .5f - FMath::Abs(.5f - fraction)), 1.f / 3.f) *
		FMath::Sin(fraction * PI * 2.f) *
		4.813f; // normalize to [-1..1]
}