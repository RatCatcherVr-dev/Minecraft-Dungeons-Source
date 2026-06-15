#include "Dungeons.h"
#include "DynamicBeam.h"
#include "Assets/DungeonsAssetManager.h"
#include "../component/PlayerExperienceComponent.h"
#include "DungeonsGameInstance.h"

ADynamicBeam::ADynamicBeam(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ADynamicBeam::UpdateBeam() {
	const auto sourceLocation = useSourceSocket
		? sourceActor->GetMesh()->GetSocketLocation(sourceSocketName)
		: sourceActor->GetActorLocation();

	beam->SetBeamSourcePoint(emitterIndex, sourceLocation, /*sourceIndex*/ 0);

	const auto targetLocation = useTargetSocket
		? targetActor->GetMesh()->GetSocketLocation(targetSocketName)
		: targetActor->GetActorLocation();

	beam->SetBeamTargetPoint(emitterIndex, targetLocation, /*targetIndex*/ 0);

	const auto distance = FVector::Dist2D(sourceActor->GetActorLocation(), targetActor->GetActorLocation());
	if (distance > distanceMax - 200.f) {
		beam->SetFloatParameter(intensityParameterName, 1.f - (distanceMax - distance) / 200.f);
	}
}

bool ADynamicBeam::CanMaintainBeam() const {
	if (!sourceActor.IsValid() || !targetActor.IsValid()) {
		return false;
	}

	const auto sourceHealthComponent = sourceActor->FindComponentByClass<UHealthComponent>();
	if (sourceHealthComponent != nullptr && sourceHealthComponent->IsNotAlive()) {
		return false;
	}

	const auto targetHealthComponent = targetActor->FindComponentByClass<UHealthComponent>();
	if (targetHealthComponent != nullptr && targetHealthComponent->IsNotAlive()) {
		return false;
	}

	return FVector::DistSquared2D(sourceActor->GetActorLocation(), targetActor->GetActorLocation()) < distanceMax * distanceMax;
}

void ADynamicBeam::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	if (!CanMaintainBeam()) {
		Despawn();
		return;
	}

	UpdateBeam();
}

void ADynamicBeam::Spawn(ABaseCharacter* source, ABaseCharacter* target) {
	sourceActor = source;
	targetActor = target;

	useSourceSocket = source->GetMesh()->DoesSocketExist(sourceSocketName);
	useTargetSocket = target->GetMesh()->DoesSocketExist(targetSocketName);

	if (beam == nullptr) 
	{
		beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), emitter, sourceActor->GetActorLocation());
	} else {
		beam->SetActive(true);
		beam->SetVisibility(true);
	}

	UpdateBeam();

	SetActorTickEnabled(true);

	OnSpawn(beam);
}

void ADynamicBeam::Despawn() {
	beam->DestroyComponent();
	Destroy();
}

void ADynamicBeam::OnSpawn_Implementation(UParticleSystemComponent* spawnedBeam) {}