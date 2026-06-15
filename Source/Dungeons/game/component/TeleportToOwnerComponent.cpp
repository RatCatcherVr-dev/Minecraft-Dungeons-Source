#include "Dungeons.h"
#include "TeleportToOwnerComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/LocationQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace teleport {

FVector AdjustToGround(const UWorld& world, const FVector& location, const AActor& actor) {
	// @attn @todo: We should make sure this is done properly inside the mob spawning as a separate project
	if (auto groundZ = locationquery::findGround(world, location, true)) {
		if (const auto* capsule = actor.FindComponentByClass<UCapsuleComponent>()) {
			return FVector(location.X, location.Y, groundZ.GetValue() + capsule->GetScaledCapsuleHalfHeight());
		}
	}
	return location;
}

void TeleportCharacter(ABaseCharacter& characterOwner, ABaseCharacter& character, const FVector& location, const FRotator& rotation, const FGameplayTag& teleportOutCue, const FGameplayTag& teleportInCue, TOptional<FGameplayEffectSpec> TeleportEffectSpec) {
	auto correctedPosition = AdjustToGround(*characterOwner.GetWorld(), location, character);

	UAbilitySystemComponent* abilitySystem = characterOwner.GetAbilitySystemComponent();

	{
		FScopedPredictionWindow tmp(abilitySystem, FPredictionKey(), false);
		// Trigger cue at start location
		FGameplayCueParameters startParams;
		startParams.Location = character.GetActorLocation();
		startParams.SourceObject = &character;

		abilitySystem->ExecuteGameplayCue(teleportOutCue, startParams);

		// Trigger cue at end location
		FGameplayCueParameters endParams;
		endParams.Location = correctedPosition;
		endParams.SourceObject = &character;

		abilitySystem->ExecuteGameplayCue(teleportInCue, endParams);
	}

	if (TeleportEffectSpec) {
		if (auto mobAbilitySystem = character.GetAbilitySystemComponent()) {
			mobAbilitySystem->ApplyGameplayEffectSpecToSelf(TeleportEffectSpec.GetValue());
		}
	}

	character.SetActorLocation(correctedPosition, false, nullptr, ETeleportType::ResetPhysics);
	character.SetActorRotation(rotation);
}

TOptional<FVector> TryFindSpawnLocation(ABaseCharacter& owner, float offset, const FVector& dir) {
	FHitResult res;
	FCollisionObjectQueryParams queryParams;
	queryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	auto& world = *owner.GetWorld();

	FVector spawnLocation = owner.GetActorLocation() + dir * offset;

	if (world.LineTraceSingleByObjectType(res, owner.GetActorLocation(), spawnLocation, queryParams)) {
		spawnLocation = res.ImpactPoint;
	}

	if (auto maybeLocation = locationquery::getRandomLocationAround(world, owner, spawnLocation)) {
		return maybeLocation;
	}

	//If finding a location at the desired offset and direction fails, just try to spawn somewhere around the owner.
	const float retrySpawnRadius = 500.f;
	return locationquery::getRandomLocationAround(world, owner, owner.GetActorLocation(), retrySpawnRadius);
}

}

UTeleportToOwnerComponent::UTeleportToOwnerComponent() {
	TeleportSpec = []() { return TOptional<FGameplayEffectSpec>(); };
	TeleportOutCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Teleport");
	TeleportInCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Teleport");
}

void UTeleportToOwnerComponent::AddCharacter(ABaseCharacter& character) {
	Characters.Emplace(&character);
	character.OnDeath.AddUObject(this, &UTeleportToOwnerComponent::OnCharacterDied);
	if (GetOwner()->HasAuthority()) {
		if (!DistanceCheckTimerHandle.IsValid()) {
			GetWorld()->GetTimerManager().SetTimer(DistanceCheckTimerHandle, FTimerDelegate::CreateUObject(this, &UTeleportToOwnerComponent::TeleportIfFarAway), 2.0f, true);
		}
	}
}

void UTeleportToOwnerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTeleportToOwnerComponent, Characters);
}

void UTeleportToOwnerComponent::TeleportIfFarAway() {
	const auto owner = Cast<ABaseCharacter>(GetOwner());
	const auto teleportRangeSquared = AutoTeleportDistanceThreshold * AutoTeleportDistanceThreshold;
	for (auto& character : Characters) {
		if (character.IsValid()) {
			//Weak-ptrs, and they should be continuously cleared from the Characters array as they are killed so should not have to check for more state here.
			if (actorquery::getActorDistanceSquared(owner, character.Get()) >= teleportRangeSquared) {
				if (TOptional<FVector> maybeLocation = teleport::TryFindSpawnLocation(*owner, 0, FVector(0))) {
					teleport::TeleportCharacter(*owner, *character.Get(), maybeLocation.GetValue(), owner->GetActorRotation(), TeleportOutCue, TeleportInCue, TeleportSpec());
					break; //Only teleport one out of range character per timer execution.
				}
			}
		}
	}
}

void UTeleportToOwnerComponent::OnCharacterDied() {
	Characters.RemoveAllSwap([](TWeakObjectPtr<ABaseCharacter>& character) {
		return !character.IsValid() || character->GetHealthComponent()->IsNotAlive();
	});

	if (Characters.Num() <= 0 && DistanceCheckTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(DistanceCheckTimerHandle);
	}
}
