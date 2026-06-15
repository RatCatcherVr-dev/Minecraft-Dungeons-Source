// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "FireworkBombInstance.h"
#include <Components/SphereComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "../power/ItemPowerEffectDefs.h"
#include "game/component/ReplicatedRandomSeedComponent.h"



UFireworkBombGameplayEffect::UFireworkBombGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion")));
}


AFireworkBombProjectile::AFireworkBombProjectile() {
	ExplosionTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.FireWorkBomb.Projectile");
}

void AFireworkBombProjectile::BeginPlay() {
	Super::BeginPlay();
	if (auto sphere = FindComponentByClass<USphereComponent>()) {
		sphere->OnComponentBeginOverlap.AddDynamic(this, &AFireworkBombProjectile::OnOverlapBegin);
	}
}

void AFireworkBombProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(AFireworkBombProjectile::StaticClass())) {
		return;
	}

	OnPreDestroy();

	if (HasAuthority()) {
		auto owner = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = owner->GetAbilitySystemComponent();

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UFireworkBombGameplayEffect>(abilitySystem);
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
		context->AddInstigator(GetOwner(), this);
		context->AddOrigin(GetOwner()->GetActorLocation());

		for (auto mob : actorquery::getNearbyActors<AMobCharacter>(this, ExplosionRadius)) {
			if (owner->CanDamageTarget(mob) && mob->IsTargetable()) {
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent());
			}
		}
	}
	Destroy();
}

AFireworkBombActor::AFireworkBombActor() {
	ExplosionTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.FireWorkBomb.PrimaryExplosion");
}

void AFireworkBombActor::BeginPlay() {
	Super::BeginPlay();

	if (auto sphere = FindComponentByClass<USphereComponent>()) {
		sphere->OnComponentBeginOverlap.AddDynamic(this, &AFireworkBombActor::OnOverlapBegin);
	}
}

void AFireworkBombActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(AFireworkBombActor::StaticClass())) {
		return;
	}

	OnPreDestroy();

	if (HasAuthority()) {
		auto owner = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = owner->GetAbilitySystemComponent();

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UFireworkBombGameplayEffect>(abilitySystem);
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		spec.SetSetByCallerMagnitude(effects::HealthName, -BombDamage);
		context->AddInstigator(GetOwner(), this);
		context->AddOrigin(GetOwner()->GetActorLocation());

		for (auto actor : actorquery::getNearbyActors<AMobCharacter>(GetOwner(), ExplosionRadius)) {
			if (owner->CanDamageTarget(actor) && actor->IsTargetable()) {
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, actor->GetAbilitySystemComponent());
			}
		}
		SetProjectilesTransform();
		for (auto actor : projectilegeneration::Spawn(GetOwner(), ProjectileElements)) {
			auto firework = Cast<AFireworkBombProjectile>(actor);
			firework->Damage = ProjectileDamage;
		}
	}
	Destroy();
}

void AFireworkBombActor::SetProjectilesTransform() {
	for (auto& proj : ProjectileElements) {
		auto transform = GetActorTransform();
		transform.SetLocation(transform.GetLocation() + FVector(0.f, 0.f, 100.f));
		proj.SpawnTransform = transform;
	}
}

AFireworkBombInstance::AFireworkBombInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	RandomComponent = CreateDefaultSubobject<UReplicatedRandomSeedComponent>(TEXT("Random"));
}

void AFireworkBombInstance::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker<AFireworkBombInstance>::AddInstance(GetWorld(), this);
	RandomComponent->SetSeed(FMath::Rand());

	ProjGenerator.TransformGen = [](Random*) { return FTransform(); };
	ProjGenerator.VelocityGen = projectilegeneration::RandomVelocityAround(FloatRange(ProjectileItemSpec.VelocityMin, ProjectileItemSpec.VelocityMax));
	ProjGenerator.TypeGen = [&](Random* r) { return ProjectileItemSpec.ClassesToSpawn[FloatRange(0, ProjectileItemSpec.ClassesToSpawn.Num() - 1).random(r)]; };
}

void AFireworkBombInstance::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
	InstanceTracker<AFireworkBombInstance>::RemoveInstance(GetWorld(), this);
}

void AFireworkBombInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	auto launchDir = GetOwner()->GetActorForwardVector() + FVector(0, 0, 0.5f);
	launchDir.Normalize();
	
	auto elements = projectilegeneration::GenerateElementsFrom(GetOwner(), RandomComponent->Stream().RandRange(ProjectileItemSpec.AmountMin, ProjectileItemSpec.AmountMax), ProjGenerator, &RandomComponent->Rand());
	if (HasAuthority()) {
		MulticastSpawnBomb(predictionKey, launchDir, elements);
	}
	else {
		SpawnBomb(launchDir, elements);
	}
}

float AFireworkBombInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
		return ProjectileDamage;
	case EItemStats::LowestDamage:
		return BombDamage;
	}
	return -1;
}

void AFireworkBombInstance::SpawnBomb(const FVector& launchDir, const TArray<FProjectileElement>& projectileElements) {
	const auto transform = GetOwner()->GetTransform();
	Bomb = GetWorld()->SpawnActorDeferred<AFireworkBombActor>(BombActorClass, transform, GetOwner(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Bomb->ProjectileElements = projectileElements;
	const auto multiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	Bomb->BombDamage = BombDamage * multiplier;
	Bomb->ProjectileDamage = ProjectileDamage * multiplier;
	UGameplayStatics::FinishSpawningActor(Bomb.Get(), transform);
	auto projectileMovement = Bomb.IsValid() ? Bomb->FindComponentByClass<UProjectileMovementComponent>() : nullptr;
	if (projectileMovement) {
		projectileMovement->Velocity = launchDir * BombLaunchSpeed;
	}
}

void AFireworkBombInstance::MulticastSpawnBomb_Implementation(FPredictionKey predictionKey, const FVector& launchDir, const TArray<FProjectileElement>& projectileElements) {
	if (HasAuthority() || !predictionKey.IsLocalClientKey() ) {
		SpawnBomb(launchDir, projectileElements);
	}
}
