#include "Dungeons.h"
#include "BumpableComponent.h"
#include "MassComponent.h"
#include "game/util/Pushback.h"
#include "game/actor/character/player/PlayerCharacter.h"

UBumpableComponent::UBumpableComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	m_Parent_CharacterMovementComponent = nullptr;
	m_Parent_bumplableMassComponent = nullptr;
}

void UBumpableComponent::BeginPlay() {
	Super::BeginPlay();

	UMassComponent* bumplableMassComponent = GetOwner()->FindComponentByClass<UMassComponent>();
	UCharacterMovementComponent* characterMovementComponent = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();

	if (bumplableMassComponent && characterMovementComponent)
	{
		m_Parent_CharacterMovementComponent = characterMovementComponent;
		m_Parent_bumplableMassComponent = bumplableMassComponent;
		GetOwner()->OnActorHit.AddDynamic(this, &UBumpableComponent::OnActorHit);
	}

}

void UBumpableComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	HasBumpedThisTick = false;
}

TAutoConsoleVariable<int32> CVarBumpableNew(
	TEXT("Dungeons.BumpingActors.New"),
	1,
	TEXT("0 = use old behavior. 1 = use new behavior."),
	ECVF_Cheat
);

TAutoConsoleVariable<float> CVarBumpableImpulseScale(
	TEXT("Dungeons.BumpingActors.ImpulseScale"),
	1.0f,
	TEXT("Scale the bump impulse. 1.0 = similar to old behavior."),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarBumpableOnlyPlayers(
	TEXT("Dungeons.BumpingActors.OnlyPlayers"),
	0,
	TEXT("0 = everything can bump. 1 = only players can bump."),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarBumpableStopMovement(
	TEXT("Dungeons.BumpingActors.StopMovement"),
	0,
	TEXT("Stops movement when bumping. 0 = don't stop. 1 = stop movement. 2 = stop movement and pathing (old behavior)."),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarBumpablePlayerStopMovement(
	TEXT("Dungeons.BumpingActors.PlayerStopMovement"),
	1,
	TEXT("Stops movement when bumping with player. 0 = don't stop. 1 = stop movement. 2 = stop movement and pathing (old behavior)."),
	ECVF_Cheat
);

void UBumpableComponent::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	if (HasBumpedThisTick) {
		return;
	}
	if (OtherActor == SelfActor) {
		return;
	}

	auto bumpable = SelfActor;

	if (m_Parent_bumplableMassComponent->IsImmovable()) {
		return;
	}

	UMassComponent* bumperMassComponent = OtherActor->FindComponentByClass<UMassComponent>();
	if (bumperMassComponent) {
		if (CVarBumpableNew.GetValueOnGameThread() == 1) {
			bool bumpWithPlayer = Cast<APlayerCharacter>(OtherActor) != nullptr;
			if (CVarBumpableStopMovement.GetValueOnGameThread() == 2 || (bumpWithPlayer && CVarBumpablePlayerStopMovement.GetValueOnGameThread() == 2)) {
				m_Parent_CharacterMovementComponent->StopActiveMovement();
			}
			else if (CVarBumpableStopMovement.GetValueOnGameThread() == 1 || (bumpWithPlayer && CVarBumpablePlayerStopMovement.GetValueOnGameThread() == 1)) {
				m_Parent_CharacterMovementComponent->StopMovementKeepPathing();
			}

			float deltaTime = GetWorld()->GetDeltaSeconds();
			float timeScaling = FMath::Clamp(deltaTime, 1 / 60.0f, 1 / 30.0f);

			FVector impulse = Hit.ImpactNormal * 12000.0f * m_Parent_bumplableMassComponent->GetPushMultiplier() * timeScaling;
			impulse.Z = 0;
			auto scaledImpulse = impulse * CVarBumpableImpulseScale.GetValueOnGameThread();
			if (CVarBumpableOnlyPlayers.GetValueOnGameThread() == 0 || bumpWithPlayer) {
				m_Parent_CharacterMovementComponent->AddImpulse(scaledImpulse, true);

				HasBumpedThisTick = true;
			}
		}
		else {
			m_Parent_CharacterMovementComponent->StopActiveMovement();
			m_Parent_CharacterMovementComponent->Launch(Hit.ImpactNormal * 100.0f * m_Parent_bumplableMassComponent->GetPushMultiplier());
			m_Parent_CharacterMovementComponent->HandlePendingLaunch();
		}
	}
}

