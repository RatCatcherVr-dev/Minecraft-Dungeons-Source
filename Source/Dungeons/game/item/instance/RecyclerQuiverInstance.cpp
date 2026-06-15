#include "Dungeons.h"
#include "RecyclerQuiverInstance.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "UnrealNetwork.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/PlayerCharacter.h"

void ARecyclerQuiverInstance::BeginPlay() {
	Super::BeginPlay();
}

int ARecyclerQuiverInstance::GetDisplayCount() const {
	return 0;
}

bool ARecyclerQuiverInstance::CanActivate() const {
	return false;
}

void ARecyclerQuiverInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetOwner();
	if (!HasAuthority())
		return;
	Super::Activate(predictionKey);
}

void ARecyclerQuiverInstance::OnHit() {
	auto role = Role;
}

void ARecyclerQuiverInstance::OnSetAddArrow_Implementation(bool addArrow) {
	bCanAddArrow = true;
	bShouldAddArrow = addArrow;
	TryActivate();
}

void ARecyclerQuiverInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARecyclerQuiverInstance, bCanAddArrow);
}