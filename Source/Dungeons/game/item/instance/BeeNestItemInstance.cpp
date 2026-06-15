#include "Dungeons.h"
#include "Net/UnrealNetwork.h"
#include "BeeNestItemInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "MobSummonItem.h"
#include <AbilitySystemGlobals.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/ActorQuery.h"
#include "game/item/power/ItemPowerStats.h"
#include "util/Algo.h"

#include "RuntimeMeshComponent.h"
ABeeNestItemInstance::ABeeNestItemInstance() {
	PowerEffects = { USummonDamageIncrease::StaticClass() };
	bHasManualCooldownActivation = true;
}


void ABeeNestItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeeNestItemInstance, BeeNest)
}

void ABeeNestItemInstance::OnRep_BeeNest() {
	if (BeeNest) {
		OutStandingPrediction = false;
	}
}

bool ABeeNestItemInstance::IsBusy() const {
	return OutStandingPrediction || BeeNest;
}

void ABeeNestItemInstance::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (HasAuthority()) {
		if(BeeNest) {
			BeeNest->Expire();
		}

		for (const auto& maybeBee : SummonCarryOver) {
			if (maybeBee.IsValid()) maybeBee->Kill();
		}
	}
}

bool ABeeNestItemInstance::CanActivate() const {
	return Super::CanActivate() && !IsBusy();
}

void ABeeNestItemInstance::Activate(const FPredictionKey& predictionKey) {
	if (HasAuthority()) {

		auto startLocation = GetOwner()->GetActorLocation();

		FHitResult result;
		if (GetWorld()->LineTraceSingleByChannel(result, startLocation, startLocation - FVector(0, 0, 1000.f), (ECollisionChannel)ECustomTraceChannels::TerrainOnly)) {
			startLocation = result.Location;
		}

		FTransform transform;
		transform.SetLocation(startLocation);
		transform.SetRotation(FQuat::MakeFromEuler(FVector(0, 0, FMath::RandHelper(4) * 90.f)));

		BeeNest = GetWorld()->SpawnActorDeferred<ABeeNest>(BeesNestClass, transform, GetOwner(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (BeeNest) {
			auto* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
			BeeNest->InitFromItem(MaxNumBees, ItemPower, SpawnDelayRangeMin, SpawnDelayRangeMax);
			SummonCarryOver.Empty();
			BeeNest->OnExpired.AddUObject(this, &ABeeNestItemInstance::OnBeenestExpired);
			UGameplayStatics::FinishSpawningActor(BeeNest, transform);
			BeeNest->SetLifeSpan(GetItemType().getDurationSeconds());

			/* Attach this to the player floor */
			FHitResult Hit;
			FVector end = startLocation;
			end.Z -= 10000;
			TArray<AActor*> ActorsToIgnore;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), ActorsToIgnore);
			ActorsToIgnore.Add(GetOwner());
			UKismetSystemLibrary::LineTraceSingle(GetOwner(), startLocation, end, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);
			if (Hit.Component.IsValid() || Hit.Actor.IsValid())
			{
				FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
				if (Hit.Component.IsValid() && !Hit.Component.Get()->IsA<URuntimeMeshComponent>())
				{
					BeeNest->AttachToComponent(Hit.Component.Get(), Rules);
				}
				else if (Hit.Actor.IsValid())
				{
					BeeNest->AttachToActor(Hit.Actor.Get(), Rules);
				}

				FVector newTotemLocation = Hit.ImpactPoint;
				BeeNest->SetActorLocation(newTotemLocation);
			}
		}
	} else {
		OutStandingPrediction = true;
	}
	Super::Activate(predictionKey);
}

void ABeeNestItemInstance::OnBeenestExpired() {
	if (BeeNest) {
		SummonCarryOver = algo::map_tarray(BeeNest->GetBees(), RETLAMBDA(TWeakObjectPtr<AMobCharacter>(it)));
	}

	BeeNest = nullptr;
	Cooldown().TriggerCooldown(CalculateCooldown());
	
}

void ABeeNestItemInstance::PredictionCaughtUp() {
	OutStandingPrediction = false;
}

float ABeeNestItemInstance::GetStats(EItemStats stats) const {
	return AMobSummonItem::GetStatsForEntityType(stats, BeesNestClass.GetDefaultObject()->GetBeeType());
}