#include "Dungeons.h"
#include "BuffGrantTotem.h"
#include "UnrealNetwork.h"
#include "game/component/AreaBuffComponent.h"

ABuffGrantTotemActor::ABuffGrantTotemActor() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ABuffGrantTotemActor::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(this, FName("Buff totem buff component"));
		OwnerAreaBuffComponent->Effect = Effect;
		OwnerAreaBuffComponent->AreaSize = Radius;
		OwnerAreaBuffComponent->EffectApplicationOwner = Cast<ABaseCharacter>(GetOwner());
		PreBuffComponentBeginPlay(OwnerAreaBuffComponent.Get());
		OwnerAreaBuffComponent->RegisterComponent();
	}

	if (Duration > -1) {
		GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &ABuffGrantTotemActor::TryStartDestroyCountdown, Duration);
	}
}

void ABuffGrantTotemActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	TryNotifyWeakenedVitality();
}

void ABuffGrantTotemActor::TryStartDestroyCountdown() {
	Super::TryStartDestroyCountdown();

	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

void ABuffGrantTotemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuffGrantTotemActor, Duration);
}

float ABuffGrantTotemActor::GetHealthFraction() {
	if (Duration > -1) {
		const auto timeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(DestructionTimerHandle);

		return timeRemaining / Duration;
	}
	return 1.0f;
}

