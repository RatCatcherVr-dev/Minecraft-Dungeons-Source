// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ChargedRedstoneMinesInstance.h"
#include "util/CharacterQuery.h"
#include "game/util/ComponentUtils.h"
#include "Net/UnrealNetwork.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include <NavigationSystem.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

#include "RuntimeMeshComponent.h"

AChargedRedstoneMinesInstance::AChargedRedstoneMinesInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
}


float AChargedRedstoneMinesInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::ProjectilesPerAttack:
		return MineSpawnAmount;
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return DamagePerMine;
	}
	return -1;
}

void AChargedRedstoneMinesInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	if (Role == ROLE_Authority) {
		float numMines = SpawnMines();
		Cooldown().TriggerCooldown(CalculateCooldown() * numMines/MineSpawnAmount, predictionKey);
	}
	else {
		Cooldown().TriggerCooldown(CalculateCooldown(), predictionKey);
	}
}

int AChargedRedstoneMinesInstance::SpawnMines() {
	const auto nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	auto* owner = GetCharacterOwner();
	auto ownerAbilitySystem = owner->GetAbilitySystemComponent();
	const auto spawnDirection = owner->GetActorForwardVector().RotateAngleAxis(MineSpawnAngle, FVector::UpVector) * MineSpawnDistance;
	const float angleIncrement = MineSpawnCone / (MineSpawnAmount - 1);

	const FVector blockOffset = FVector::UpVector * 100.0f;

	const float damage = GetPowerEffect()->GetMultiplier(ItemPower) * DamagePerMine;
	auto hostileSpec = effects::CreateGameplayEffectSpec<UChargedRedstoneMineDamageGameplayEffect>(ownerAbilitySystem, effects::HealthName, -damage, owner, this, owner->GetActorLocation(), ItemPower);
	auto friendlySpec = effects::CreateGameplayEffectSpec<UChargedRedstoneMineDamageGameplayEffect>(ownerAbilitySystem, effects::HealthName, -damage * TeamDamageFactor, owner, this, owner->GetActorLocation(), ItemPower);

	int SpawnedMines = 0;

	for (int index = 0; index < MineSpawnAmount; index++) {
		const FTransform transform{
			FRotator::ZeroRotator,
			GetOwner()->GetActorLocation() + spawnDirection.RotateAngleAxis(index * angleIncrement - MineSpawnCone / 2, FVector::UpVector),
			FVector::OneVector
		};

		FNavLocation result;


		if (nav->ProjectPointToNavigation(transform.GetLocation(), result, FVector(50.0f, 50.0f, 200.0f))) {
			FHitResult res;
			result.Location = FVector(FIntVector(result.Location * 0.01f) * 100) + FVector(50.0f);

			FCollisionObjectQueryParams params;
			params.AddObjectTypesToQuery(ECC_WorldStatic);
			if (GetWorld()->LineTraceSingleByObjectType(res, result.Location + blockOffset, result.Location - blockOffset * 5.f, params)) {
				result.Location = res.ImpactPoint;
			}

			FActorSpawnParameters spawnParameters;
			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			spawnParameters.Owner = this;

			if (AChargedRedstoneMineActor* item = GetWorld()->SpawnActor<AChargedRedstoneMineActor>(ChargedRedstoneMineBP, result.Location, FRotator::ZeroRotator, spawnParameters)) {
				SpawnedMines++;
				item->Init(hostileSpec, friendlySpec);
				item->SetLifeSpan(MineAliveTime);

				// Attach this mine at the floor so if we spawn it on a lift it goes with it
				FHitResult Hit;
				FVector end = result.Location;
				end.Z -= 10000;
				TArray<AActor*> ActorsToIgnore;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), ActorsToIgnore);
				ActorsToIgnore.Add(GetOwner());
				UKismetSystemLibrary::LineTraceSingle(GetOwner(), result.Location, end, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
				if (Hit.Component.IsValid() || Hit.Actor.IsValid())
				{
					FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
					if (Hit.Component.IsValid())
					{
						if (Hit.Component.Get()->IsA(URuntimeMeshComponent::StaticClass()))
						{
							//D11.SC - Hit a runtime generated mesh of the world, network/xp[lay has issues replicating these instances across, so dont bother attaching or they will end up lost if UE cant work out its equivilent on the client (its only required for moving UE objects anyway)
							continue;
						}
						item->AttachToComponent(Hit.Component.Get(), Rules);
					}
					else if (Hit.Actor.IsValid())
					{
						item->AttachToActor(Hit.Actor.Get(), Rules);
					}
				}
			}
		}
	}

	return SpawnedMines;
}

UChargedRedstoneMineDamageGameplayEffect::UChargedRedstoneMineDamageGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Damage.Item")), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}
