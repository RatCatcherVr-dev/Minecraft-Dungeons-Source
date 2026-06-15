#include "PowerStation.h"
#include "UnrealNetwork.h"


APowerStation::APowerStation(const FObjectInitializer& objectInitializer) : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	PowerStationMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PowerStationMesh"));
	PowerStationMesh->SetupAttachment(RootComponent);
}

void APowerStation::BeginPlay()
{
	Super::BeginPlay();

	PowerStationMesh->OnComponentBeginOverlap.AddDynamic(this, &APowerStation::PowerStationOverlapped);
}

void APowerStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APowerStation, State)
}

EPowerStationState APowerStation::GetPowerStationState() const
{
	return State;
}

void APowerStation::OnRep_PowerStationState()
{
	OnPowerStationStateChange.Broadcast(State);
	OnStateChanged();
}

void APowerStation::ActivatePowerStation()
{
	if (HasAuthority() && State != EPowerStationState::Active) {
		State = EPowerStationState::Active;
		OnRep_PowerStationState();
	}
}

void APowerStation::DisablePowerStation()
{
	State = EPowerStationState::Inactive;
	OnRep_PowerStationState();
}
