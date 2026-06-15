#include "Dungeons.h"
#include "BackpackComponent.h"
#include "PickupComponent.h"
#include "RelocateComponent.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include <UnrealNetwork.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/Conversion.h"
#include "game/util/LocationQuery.h"
#include "Dungeons/DungeonsGameInstance.h"

#include <limits>

UMochilaComponent::UMochilaComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;
}

void UMochilaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMochilaComponent, ActorClass);
}

void UMochilaComponent::BeginPlay() {
	Super::BeginPlay();

	if (auto* characterOwner = Cast<ABaseCharacter>(GetOwner())) {
		characterOwner->OnTeamChanged.AddUObject(this, &UMochilaComponent::Drop);
	}
}

void UMochilaComponent::Store(TSubclassOf<class AActor> mountedBlueprint, EntityType entityType, const FVector& location, const FVector& firstLocation) {
	Drop();

	ActorClass = mountedBlueprint;
	OnRep_ActorClass();
	
	StoredEntityType = entityType;
	StoredEntityLocation = location;
	StoredEntityFirstLocation = firstLocation;
}

bool UMochilaComponent::IsEmpty() const {
	return !StoredEntityType.IsSet();
}

bool UMochilaComponent::Has(EntityType entityType) const {
	return !IsEmpty() && StoredEntityType.GetValue() == entityType;
}

EntityType UMochilaComponent::Get() const {
	return StoredEntityType.Get(EntityType::Undefined);
}

void UMochilaComponent::EndPlay(EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	if (reason == EEndPlayReason::Destroyed && GetOwnerRole() == ROLE_Authority) {
		Drop();
	}
}


game::mobspawn::MobAction DisableDropMobCollision() {
	return [](AMobCharacter& mob) {mob.SetActorEnableCollision(false); };
}

void UMochilaComponent::Drop() {
	const auto world = GetWorld();
	const auto owner = GetOwner();

	if (!ActorClass || IsEmpty() || !world || !owner) {
		return;
	}

	// find a suitable location around the owner, starting at the back
	auto transformProvider = [owner, world, currentTry = 0]() mutable {
		constexpr int maxTries = 4;
		constexpr float offsetPerTry = 360.f / maxTries;
		static const auto invalidTransform = FTransform{ FVector{ std::numeric_limits<float>::max() } };

		const FVector offsetVector = owner->GetActorForwardVector().RotateAngleAxis(offsetPerTry * currentTry, FVector::UpVector).GetUnsafeNormal() * 100.f;
		const auto location = owner->GetActorLocation() - offsetVector;

		currentTry = (currentTry + 1) % maxTries;

		const auto maybeGround = locationquery::findGround(*world, location);
		if (!maybeGround) {
			return invalidTransform;
		}

		if (const auto ownerCapsuleComponent = owner->FindComponentByClass<UCapsuleComponent>()) {
			if (maybeGround.GetValue() > owner->GetActorLocation().Z - ownerCapsuleComponent->GetScaledCapsuleHalfHeight() + 100.f) {
				return invalidTransform;
			}
		}

		return  FTransform{ location };
	};

	// if the key fails to spawn the safest thing is to do nothing and keep it in the backpack, unless the owner is dead
	if (const auto mob = spawnNow(*world, StoredEntityType.GetValue(), transformProvider, game::mobspawn::configs::DefaultNoVariants(true).Action(DisableDropMobCollision()))) {
		mob->FindComponentByClass<UPickupComponent>()->StoredLocation = StoredEntityLocation;
		mob->FindComponentByClass<URelocateComponent>()->RelocateLocation = StoredEntityFirstLocation.GetValue();

		ActorClass = nullptr;
		mob->SetActorEnableCollision(true);
		mob->GetRootComponent()->SetVisibility(true, true);
		OnRep_ActorClass();
	}
}

void UMochilaComponent::DropAtStoredLocation() {
	if (IsEmpty()) {
		return;
	}

	if (const auto* mob = spawnNow(
		*GetWorld(),
		StoredEntityType.GetValue(),
		game::mobspawn::providers::Location(StoredEntityLocation.GetValue()),
		game::mobspawn::configs::DefaultNoVariants(true)
	)) {
		mob->FindComponentByClass<URelocateComponent>()->RelocateLocation = StoredEntityFirstLocation.GetValue();
	}
	

	Clear();
}

void UMochilaComponent::ApplyDamage(const FGameplayEffectSpec& spec) {	
	FGameplayTagContainer tags;
	spec.GetAllAssetTags(tags);
	
	if (tags.HasTag(damageTag::weak()) || tags.HasTag(damageTag::drowning())) {
		return;
	}
	
	if (
		!IsEmpty() &&
		(tags.HasTagExact(damageTag::killzone()) || Cast<ABaseCharacter>(GetOwner())->GetWorldState() != ECharacterWorldState::InWorld)
	) {
		DropAtStoredLocation();
	} else {
		Drop();
	}
}

void UMochilaComponent::Clear() {
	if (ActorClass) {
		ActorClass = nullptr;
		OnRep_ActorClass();
	} 
}

void UMochilaComponent::OnRep_ActorClass() {
	
	if (ABaseCharacter* pCharOwner = Cast<ABaseCharacter>(GetOwner()))
	{
		//Base character backpack pickup
		UChildActorComponent* pBackPackComp = pCharOwner->GetBackPackComponent();

		if (!pBackPackComp) {
			UE_LOG(LogTemp, Warning, TEXT("Actor without backpack tried to get a key"));
			return;
		}

		if (ActorClass) {
			pCharOwner->SetBackPackComponentAttached(true);
			pBackPackComp->SetChildActorClass(ActorClass);
			pBackPackComp->SetVisibility(true, true);
		}
		else {
			pBackPackComp->DestroyChildActor();
			pBackPackComp->GetComponentInstanceData();
			pCharOwner->SetBackPackComponentAttached(false);

			StoredEntityType.Reset();

			if (DropParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DropParticleSystem, GetOwner()->GetActorTransform());
			}
		}

	}
	else
	{
		const auto candidateComponents = GetOwner()->GetComponentsByTag(UChildActorComponent::StaticClass(), FName { "Backpack" });
		if (candidateComponents.Num() <= 0) {
			UE_LOG(LogTemp, Warning, TEXT("Actor without backpack tried to get a key"));
			return;
		}
		const auto backpack = Cast<UChildActorComponent>(candidateComponents[0]);

		if (ActorClass) {
			backpack->SetChildActorClass(ActorClass);
			backpack->SetVisibility(true, true);
		} else {
			backpack->DestroyChildActor();
			/* delete */ backpack->GetComponentInstanceData();

			StoredEntityType.Reset();

			if (DropParticleSystem) 
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DropParticleSystem, GetOwner()->GetActorTransform());
			}
		}
	}
}
