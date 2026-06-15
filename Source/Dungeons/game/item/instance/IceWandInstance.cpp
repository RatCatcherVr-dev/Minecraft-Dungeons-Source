#include "Dungeons.h"
#include "IceWandInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"

AIceWandInstance::AIceWandInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	bCanFail = true;
}

void AIceWandInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	// Early checks
	if (!GetOwner() || !GetOwner()->HasAuthority() || !IceblockActor) {
		return;
	}

	// Get the forward spawn point of the block
	FVector targetSpawnPos = GetOwner()->GetActorLocation() + (IceSpawnDistance * GetOwner()->GetActorForwardVector()) + (FVector::UpVector * IceSpawnHeight);

	// Spawn iceblock at an elevation up from the targetted point.
	if (AFallingIceActor* iceblock = GetWorld()->SpawnActor<AFallingIceActor>(IceblockActor)) {
		iceblock->SetOwner(GetOwner());
		iceblock->SetActorLocation(targetSpawnPos);

		const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
		iceblock->SetScaledExplosionDamage(IceDamageOverride * ItemPowerMultiplier);

		ActivationSucceeded(predictionKey);
	}

}

float AIceWandInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::LowestDamage:
	case EItemStats::HighestDamage:
		return IceDamageOverride;
	default:
		break;
	}

	return -1;
}
