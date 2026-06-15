#include "Dungeons.h"
#include "AncientGuardianComponent.h"
#include "DungeonsGameMode.h"
#include "Kismet/KismetMathLibrary.h"

UAncientGuardianComponent::UAncientGuardianComponent()
{
}

void UAncientGuardianComponent::BeginPlay()
{
	Super::BeginPlay();

	thorns = GetOwner()->FindComponentByClass<UAncientGuardianThorns>();

	for (auto alcove : alcoves)	{
		if (alcove) {
			alcoveLocations.Add(alcove->GetActorLocation());
		}
	}

	for (int i = 0; i < mineFormations.Num(); ++i) {
		for (auto point : mineFormations[i].spawnPoints)
		{
			if (point)
			{
				mineFormations[i].positions.Add(point->GetActorLocation());
			}
		}
	}
}

void UAncientGuardianComponent::SetAncientGuardianState(EAncientGuardianState newState)
{
	if (GetOwner()->HasAuthority()) {
		if ((ancientGuardianState == EAncientGuardianState::Return && newState != EAncientGuardianState::Normal) || (ancientGuardianState == EAncientGuardianState::Retreating && newState != EAncientGuardianState::Retreated)) {
			return;
		}
		if (newState == EAncientGuardianState::Retreated && !GetWorld()->GetTimerManager().IsTimerActive(returnTimerHandle)) {
			GetWorld()->GetTimerManager().SetTimer(returnTimerHandle, this, &UAncientGuardianComponent::SetReturnState, retreatTime, false);
		}
		ancientGuardianState = newState;
		OnRep_StateChange();
	}
}

void UAncientGuardianComponent::HandleSpikeEnchantment()
{
	if (thorns) {
		thorns->SetEnabled(ancientGuardianState != EAncientGuardianState::NotSpiking);
	}
}

FVector UAncientGuardianComponent::GetNextReatreatLocation()
{
	return retreatLocation;
}

void UAncientGuardianComponent::SetNextReatreatLocation()
{
	int randomIndex = rnd.nextInt(alcoveLocations.Num());
	retreatLocation = alcoveLocations[randomIndex];
}

void UAncientGuardianComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAncientGuardianComponent, ancientGuardianState)
}

void UAncientGuardianComponent::OnRep_StateChange()
{
	if (ancientGuardianState == EAncientGuardianState::Retreating) {
		AMobCharacter* mob = Cast<AMobCharacter>(GetOwner());
		mob->ApplyInvulnerability(-1.0f, false);
		mob->SetTargetable(false);
	}
	else if (ancientGuardianState == EAncientGuardianState::Return) {
		AMobCharacter* mob = Cast<AMobCharacter>(GetOwner());
		mob->RemoveInvulnerability();
		mob->SetTargetable(true);
	}
	OnAncientGuardianStateChanged.Broadcast(ancientGuardianState);
	HandleSpikeEnchantment();
}

void UAncientGuardianComponent::SetReturnState() {
	GetWorld()->GetTimerManager().ClearTimer(returnTimerHandle);
	SetAncientGuardianState(EAncientGuardianState::Return);
}