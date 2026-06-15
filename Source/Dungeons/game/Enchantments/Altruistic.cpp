#include "Dungeons.h"
#include "Altruistic.h"
#include "game/util/ActorQuery.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MoveToTargetMovementComponent.h"
#include "GameplayEffectExtension.h"
#include "game/util/ValueFormat.h"
#include <Components/SphereComponent.h>
#include "util/CharacterQuery.h"

UAltruistic::UAltruistic() {
	TypeId = EEnchantmentTypeID::Altruistic;
	LevelMultiplier = [this](int level) -> float {
		return PercentageOfDamageToHeal * level;
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UAltruistic::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	auto owner = GetCharacterOwner();

	if (owner->Role != ROLE_Authority)
		return;

	float healAmount = LevelMultiplier(Level) * -data.EvaluatedData.Magnitude;

	bool anyFound = false;

	auto canHeal = characterquery::can::heal(owner);
	auto filter = [&](const ABaseCharacter* target) {
		return !actorquery::is::healthMaxed(target) && actorquery::is::alive(target) && canHeal(target) && characterquery::is::targetable(target);
	};
	auto nearby = actorquery::getNearbyActors<ABaseCharacter>(owner, AffectionRadius).FilterByPredicate(filter);

	if (auto ally = actorquery::getWeakestCharacterAmong(owner->GetWorld(), nearby)) {
		MulticastSpawnSoul(ally, GetOwner()->GetActorLocation(), healAmount, (float)Level / 3.0f);
		anyFound = true;
	}

	if(anyFound){
		BroadcastEnchantmentTriggeredEvent();
	}
}

void UAltruistic::MulticastSpawnSoul_Implementation(AActor* owner, const FVector& spawnLocation, float healAmount, float magnitude) {
	if (!owner)
		return;
	
	FTransform spawnTransform;
	spawnTransform.SetLocation(spawnLocation);
	auto actor = GetWorld()->SpawnActorDeferred<AHealOnOverlapActor>(ActorClass, spawnTransform, owner, Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	actor->TargetHealthComponent = owner->FindComponentByClass<UHealthComponent>();
	actor->healAmount = healAmount;
	actor->magnitude = magnitude;
	UGameplayStatics::FinishSpawningActor(actor, spawnTransform);
}


AHealOnOverlapActor::AHealOnOverlapActor() {
	Overlap = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Overlap->SetSphereRadius(5.f);
	Movement = CreateDefaultSubobject<UMoveToTargetMovementComponent>(TEXT("Movement"));
	Movement->Acceleration = 1500.f;
	Movement->MaxSpeed = 3000.f;
	Movement->MinSpeed = 400.f;
}

void AHealOnOverlapActor::BeginPlay() {
	Super::BeginPlay();
	if (TargetHealthComponent.IsValid() && TargetHealthComponent->IsAlive()) {
		auto target = TargetHealthComponent->GetOwner();
		Movement->SetTarget(target);
		TargetHealthComponent->OnDeath.AddUObject(this, &AHealOnOverlapActor::OnTargetDied, target);
		//If for whatever reason the actor is destroyed but not dead, we also need to clean up
		TargetHealthComponent->GetOwner()->OnDestroyed.AddDynamic(this, &AHealOnOverlapActor::OnTargetDied);
	}
	else {
		Destroy();
	}
}

void AHealOnOverlapActor::OnTargetDied(AActor*) {
	Destroy();
}

void AHealOnOverlapActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	if ((OtherActor && TargetHealthComponent.IsValid()) && OtherActor == TargetHealthComponent->GetOwner()) {		
		OnHeal(TargetHealthComponent.Get());		
		Destroy();
	}
}

void AHealOnOverlapActor::OnHeal_Implementation(UHealthComponent* healthComponent) {
	healthComponent->ApplyHeal(healAmount);
	OnHealedActor.Broadcast(healthComponent->GetOwner(), magnitude);
}
