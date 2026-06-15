// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#include "Dungeons.h"
#include "NetherWartSporeGrenadeInstance.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Pushback.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/util/Pushback.h"
#include "util/Algo.hpp"
#include "game/component/ReplicatedRandomSeedComponent.h"

USporeGrenadeGameplayEffect::USporeGrenadeGameplayEffect() {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion")));
}

ASporeGrenade::ASporeGrenade() {
	InitialCollisionDisableTypes = { AMobCharacter::StaticClass() };
}

void ASporeGrenade::BeginPlay() {
	Super::BeginPlay();

	if (auto sphere = FindComponentByClass<USphereComponent>()) {
		sphere->OnComponentBeginOverlap.AddDynamic(this, &ASporeGrenade::OnOverlapBegin);
	}
	GetWorld()->GetTimerManager().SetTimer(EnableCollisionTimerHandle, this, &ASporeGrenade::EnableOverlaps, InitialCollisionDisableTime);
}

void ASporeGrenade::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(ASporeGrenade::StaticClass())) {
		return;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(EnableCollisionTimerHandle)) {
		if (algo::any_of(InitialCollisionDisableTypes, RETLAMBDA(OtherActor->IsA(it)))) {
			return;
		}
	}

	OnPreDestroy();

	OverlappedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (HasAuthority()) {
		auto owner = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = owner->GetAbilitySystemComponent();

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USporeGrenadeGameplayEffect>(abilitySystem);
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
		context->AddInstigator(owner, this);
		context->AddOrigin(owner->GetActorLocation());

		for (auto mob : actorquery::getNearbyActors<AMobCharacter>(this, ExplosionRadius)) {
			if (owner->CanDamageTarget(mob) && mob->IsTargetable()) {
				pushback::pushback(Pushback, *this, *mob);
				effects::StorePushbackInNormal(spec, pushback::getLaunchVector(Pushback, *this, *mob));
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent());
			}
		}
	}
	Destroy();
}

void ASporeGrenade::EnableOverlaps() {
	if (auto sphere = FindComponentByClass<USphereComponent>()) {
		sphere->UpdateOverlaps();
	}
}

ANetherWartSporeGrenadeInstance::ANetherWartSporeGrenadeInstance() {
	PowerEffects = { UItemArtifactDamageIncrease::StaticClass() };
	RandomComponent = CreateDefaultSubobject<UReplicatedRandomSeedComponent>(TEXT("Random"));
}

void ANetherWartSporeGrenadeInstance::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker<ANetherWartSporeGrenadeInstance>::AddInstance(GetWorld(), this);
	RandomComponent->SetSeed(FMath::Rand());
}

void ANetherWartSporeGrenadeInstance::OnSetupWithValidOwner() {
	Super::OnSetupWithValidOwner();

	ProjectileGen.TransformGen = projectilegeneration::TransformFromActor(GetOwner(), FVector(0, 0, 100.f));
	ProjectileGen.VelocityGen = projectilegeneration::RandomVelocityInCone(FloatRange(ProjectileItemSpec.VelocityMin, ProjectileItemSpec.VelocityMax), GetOwner(), ProjectileItemSpec.LaunchDirectionFromOrigin, ProjectileItemSpec.ConeAngle);
	ProjectileGen.TypeGen = [&](Random* r) { return ProjectileItemSpec.ClassesToSpawn[FloatRange(0, ProjectileItemSpec.ClassesToSpawn.Num() - 1).random(r)];
	};
}

void ANetherWartSporeGrenadeInstance::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);
	InstanceTracker<ANetherWartSporeGrenadeInstance>::RemoveInstance(GetWorld(), this);
}

float ANetherWartSporeGrenadeInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return Damage;
	}
	return -1;
}

void ANetherWartSporeGrenadeInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	auto elements = projectilegeneration::GenerateElementsFrom(GetOwner(), RandomComponent->Stream().RandRange(ProjectileItemSpec.AmountMin, ProjectileItemSpec.AmountMax), ProjectileGen, &RandomComponent->Rand());
	if (HasAuthority()) {
		MulticastSpawnGrenades(predictionKey, elements);
	}
	else {
		SpawnGrenades(elements);
	}
}

void ANetherWartSporeGrenadeInstance::MulticastSpawnGrenades_Implementation(FPredictionKey predictionKey, const TArray<FProjectileElement>& projectileElements) {
	if (HasAuthority() || !predictionKey.IsLocalClientKey()) {
		SpawnGrenades(projectileElements);
	}
}

void ANetherWartSporeGrenadeInstance::SpawnGrenades(const TArray<FProjectileElement>& projectileElements) {
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayCueParameters params;
	params.Instigator = GetOwner();
	params.Location = GetOwner()->GetActorLocation();
	abilitySystem->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.NetherwartGrenade.Launch"), EGameplayCueEvent::Executed, params);

	auto damage = Damage * GetPowerEffect()->GetMultiplier(ItemPower);
	for (auto grenadeActor : projectilegeneration::Spawn(GetOwner(), projectileElements)) {
		auto grenade = Cast<ASporeGrenade>(grenadeActor);
		grenade->Damage = damage;
	}
}

FProjectileItemSpec::FProjectileItemSpec() {
	LaunchDirectionFromOrigin = FVector(-1, 0, 1);
}
