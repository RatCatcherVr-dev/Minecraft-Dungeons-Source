#include "Dungeons.h"
#include "FishingRodItemInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/FishHookItem.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/component/EquipmentComponent.h"
#include "world/entity/MobTags.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/component/PlayerExperienceComponent.h"
#include "util/CharacterQuery.h"
#include "DungeonsGameInstance.h"

AFishingRodItemInstance::AFishingRodItemInstance() {
	bHasManualCooldownActivation = true;
	bCanFail = true;
	PowerEffects = { UStunDurationIncrease::StaticClass() };
	NonHookableMobs = { EntityType::GoldBabyKey, EntityType::SilverBabyKey };
}

float AFishingRodItemInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::StunDuration:
		return StunDuration;
	}
	return -1;
}

bool AFishingRodItemInstance::IsBusy() const {	
	return Hook != nullptr;
}

int AFishingRodItemInstance::GetDisplayCount() const {
	return 0;
}

void AFishingRodItemInstance::MulticastSpawnHook_Implementation(const FVector& location, const FRotator& rotation, AActor* closestActor, float reach, const FPredictionKey& predictionKey) {
	SpawnHook(location, rotation, closestActor, reach, predictionKey);
}

void AFishingRodItemInstance::SpawnHook(const FVector& location, const FRotator& rotation, AActor* closestActor, float reach, const FPredictionKey& predictionKey) {
	if (Hook) {
		return;
	}
	
	auto duration = StunDuration * GetPowerEffect()->GetMultiplier(ItemPower);

	//vrak: Rather ugly - but I could not find a way to adjust certain masked gameplay effects' duration throught attribute
	duration *= mDifficultyStats->GetMobStunDurationMultiplier();

	FTransform transform;
	transform.SetLocation(location);
	transform.SetRotation(rotation.Quaternion());
	Hook = GetWorld()->SpawnActorDeferred<AFishHookItem>(FishingHookActorClass, transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (Hook == nullptr) {
		if (auto character = Cast<ABaseCharacter>(GetOwner())) {
			character->SetSharedCooldown(0.0f, 0.0f);
		}
		return;
	}

	Hook->SetMaxReach(reach);
	Hook->SetStunDuration(duration);
	Hook->SetPredictionKey(predictionKey);

	if (closestActor) {
		Hook->SetTargetActor(closestActor);
	}

	Hook->Effect = Effect;

	UGameplayStatics::FinishSpawningActor(Hook, transform);
	Hook->OnHookHitTarget.BindDynamic(this, &AFishingRodItemInstance::OnHookHitTarget);
	Hook->OnHookHitPlayer.BindDynamic(this, &AFishingRodItemInstance::OnHookHitPlayer);
	Hook->OnHookonDenied.BindDynamic(this, &AFishingRodItemInstance::OnHookonDenied);
}

void AFishingRodItemInstance::OnHookThrow(const FPredictionKey predictionKey) {
	if (Hook) {
		return;
	}

	const auto reach = HookReach * game::item::power::HiddenRangeIncreaseMultiplier(ItemPower);
	AActor *closestActor = nullptr;

	auto character = Cast<ABaseCharacter>(GetOwner());
	character->SetSharedCooldown(10.0f, 10.0f);

	auto spawnRotation = character->GetActorRotation();

	for (auto actor : actorquery::getNearbyActors<AActor>(character, reach)) {
		const auto currentActor = actor;

		if (!IsCharacterTargetable(character, currentActor)) {
			continue;
		}

		const auto currentActorHierarchyPlacement = GetHierarchyPlacement(currentActor->GetClass());

		if (currentActorHierarchyPlacement < 0) {
			continue;
		}

		if (closestActor) {
			const auto closestActorHierarchyPlacement = GetHierarchyPlacement(closestActor->GetClass());

			const auto hierarchyPlacement = GetHierarchyPlacementComparison(currentActorHierarchyPlacement, closestActorHierarchyPlacement);

			if (hierarchyPlacement == EHierarchyPlacementComparison::None ||
				hierarchyPlacement == EHierarchyPlacementComparison::Below) {
				continue;
			}

			const auto& playerLocation = character->GetActorLocation();

			const auto& currentActorLocation = currentActor->GetActorLocation();
			const auto& closestActorLocation = closestActor->GetActorLocation();

			const auto currentActorDist = currentActorLocation - playerLocation;
			const auto closestActorDist = closestActorLocation - playerLocation;

			if (hierarchyPlacement == EHierarchyPlacementComparison::Same) {
				if (currentActorDist.SizeSquared() < closestActorDist.SizeSquared()) {
					if (!TargetRaycastHit(playerLocation, currentActorLocation)) {
						closestActor = currentActor;
						spawnRotation = closestActorDist.Rotation();
						break;
					}
				}
			}
			else if (hierarchyPlacement == EHierarchyPlacementComparison::Above) {
				if (!TargetRaycastHit(playerLocation, currentActorLocation)) {
					closestActor = currentActor;
					spawnRotation = currentActorDist.Rotation();
					break;
				}
			}
		}
		else {
			const auto& closestActorLocation = currentActor->GetActorLocation();
			const auto& playerLocation = character->GetActorLocation();

			const auto closestActorDist = closestActorLocation - playerLocation;
			spawnRotation = closestActorDist.Rotation();

			closestActor = currentActor;
		}
	}

	spawnRotation.Pitch = 0.0f;
	spawnRotation.Roll = 0.0f;
	
	if (HasAuthority()) {
		MulticastSpawnHook(character->GetActorLocation(), spawnRotation, closestActor, reach, predictionKey);
	}
	else {
		SpawnHook(character->GetActorLocation(), spawnRotation, closestActor, reach, predictionKey);
	}
}

int AFishingRodItemInstance::GetHierarchyPlacement(UClass* classToCheck) {
	for (auto hierarchyPlacement : TypeHierarchy) {
		if (classToCheck->IsChildOf(hierarchyPlacement.classType)) {
			return hierarchyPlacement.placement;
		}
	}
	return -1;
}

EHierarchyPlacementComparison AFishingRodItemInstance::GetHierarchyPlacementComparison(int classToCheckPlacement, int compareClassPlacement) {
	EHierarchyPlacementComparison placementComparison = EHierarchyPlacementComparison::None;
	
	if (classToCheckPlacement == compareClassPlacement) {
		placementComparison = EHierarchyPlacementComparison::Same;
	}
	else {
		placementComparison = classToCheckPlacement < compareClassPlacement ? EHierarchyPlacementComparison::Above : EHierarchyPlacementComparison::Below;
	}

	return placementComparison;
}

void AFishingRodItemInstance::OnHookHitTarget(const FPredictionKey& predictionKey, EntityType& mobEntityType) {
	if(HasAuthority()) {
		Cooldown().TriggerCooldown(CalculateCooldown());
	}

	PlayPullbackAnimation(predictionKey);

	if (auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->SetSharedCooldown(0.0f, 0.0f);
	}

	OnHookedMob(mobEntityType);

	ActivationSucceeded(predictionKey);
}

void AFishingRodItemInstance::OnHookHitPlayer(const FPredictionKey& predictionKey){
	Hook = nullptr;

	if (HasAuthority()) {
		FishingRodVisible = false;
		OnRep_FishingRodVisible();
	}
	else {
		HideFishingrod();
	}
}

void AFishingRodItemInstance::OnHookonDenied(const FPredictionKey& predictionKey) {
	PlayPullbackAnimation(predictionKey);

	if (auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->SetSharedCooldown(0.0f, 0.0f);
	}
}

void AFishingRodItemInstance::PlayPullbackAnimation(const FPredictionKey& predictionKey) {
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		if (owner->HasAuthority()) {
			owner->MulticastPlayAnimationAsDynamicMontage(FishHookPullAnimationSequence, Slot, 0, 0.2f, 1.f, 1, 0, 0, predictionKey);
		} else {
			owner->PlayAnimationAsDynamicMontage(FishHookPullAnimationSequence, Slot);
		}
	}
}

void AFishingRodItemInstance::SpawnRodActor() {
	RemoveRodActor();
	const auto playerMesh = Cast<ABaseCharacter>(GetOwner())->GetMesh();

	FishingRodChildActor = NewObject<UChildActorComponent>(playerMesh);
	FishingRodChildActor->RegisterComponent();
	FishingRodChildActor->SetChildActorClass(FishingRodActorClass);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, false);

	FishingRodChildActor->AttachToComponent(playerMesh, rules, "J_R_Weapon");
}

void AFishingRodItemInstance::RemoveRodActor() {
	if (FishingRodChildActor.IsValid()) {
		FishingRodChildActor->DestroyChildActor();
	}
}

void AFishingRodItemInstance::UpdateFishingRodVisibility() {
	if (FishingRodVisible) {
		ShowFishingRod();
	}
	else {
		HideFishingrod();
	}
}

void AFishingRodItemInstance::ShowFishingRod() {
	if (const auto* owner = GetOwner()) {
		const auto equipment = GetOwner()->FindComponentByClass<UEquipmentDisplayComponent>();
		if (equipment) {
			equipment->HideWeapons();
		}
		SpawnRodActor();
	}
}

void AFishingRodItemInstance::HideFishingrod() {
	if (const auto* owner = GetOwner()) {
		const auto equipment = GetOwner()->FindComponentByClass<UEquipmentDisplayComponent>();
		if (equipment) {
			equipment->RestoreWeaponVisibility();
		}
		RemoveRodActor();
	}
}

bool AFishingRodItemInstance::IsCharacterTargetable(const ABaseCharacter* character, const AActor* actor) const {
	// Mobs are a special case
	if (const auto mob = Cast<AMobCharacter>(actor)) {

		if( !mob->IsTargetable() || 
			hasMobTag( mob->EntityType, MobTags::HashTag_Miniboss ) || 
			hasMobTag(mob->EntityType, MobTags::HashTag_Ancient) ||
			character->IsFriendlyTowards( mob ) ||
			!characterquery::is::movable(mob) ||
			NonHookableMobs.Contains(mob->EntityType)
			) {
			return false;
		}
	}

	if (const auto* healthComponent = actor->FindComponentByClass<UHealthComponent>()) {
		return healthComponent->IsAlive();
	}

	return true;
}

bool AFishingRodItemInstance::TargetRaycastHit(const FVector& playerLocation, const FVector& targetLoaction) {
	FHitResult res;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_WorldStatic);

	if (GetWorld()->LineTraceSingleByObjectType(res, playerLocation, targetLoaction, params)) {
		return true;
	}
	return false;
}

void AFishingRodItemInstance::OnOwnerAndOwnerControllerReplicated() {
	UpdateFishingRodVisibility();
}

void AFishingRodItemInstance::OnRep_FishingRodVisible() {
	UpdateFishingRodVisibility();
}

void AFishingRodItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFishingRodItemInstance, FishingRodVisible);
}

bool AFishingRodItemInstance::CanActivate() const {
	return Super::CanActivate() && !IsBusy();
}

void AFishingRodItemInstance::Activate(const FPredictionKey& predictionKey) {
	// Unable to pass by const ref, had to pass by value
	GetWorld()->GetTimerManager().SetTimer(throwHookTimerHandle, FTimerDelegate::CreateUObject(this, &AFishingRodItemInstance::OnHookThrow, predictionKey), 0.1f, false);

	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		if (owner->HasAuthority()) {
			FishingRodVisible = true;
			OnRep_FishingRodVisible();
			owner->MulticastPlayAnimationAsDynamicMontage(FishHookThrowAnimationSequence, Slot, 0, 0.2f, 1.f, 1, 0, 0, predictionKey);
		} else {
			owner->PlayAnimationAsDynamicMontage(FishHookThrowAnimationSequence, Slot);
			ShowFishingRod();
		}
	}
	Super::Activate(predictionKey);
}

void AFishingRodItemInstance::OnHookedMob(EntityType& mobEntityType) {
	if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = character->GetStatTracker()) {
			tracker->Hooked(mobEntityType);
		}
		else {
			Client_OnHookedMob(mobEntityType);
		}
	}
}

void AFishingRodItemInstance::Client_OnHookedMob_Implementation(EntityType mobEntityType) {
	if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
		if (UStatTrackerComponent* tracker = character->GetStatTracker()) {
			tracker->Hooked(mobEntityType);
		}
	}
}