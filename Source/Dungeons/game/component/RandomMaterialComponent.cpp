#include "Dungeons.h"
#include "RandomMaterialComponent.h"
#include "game/util/Tags.h"

URandomMaterialComponent::URandomMaterialComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void URandomMaterialComponent::BeginPlay() {
	Super::BeginPlay();

	if (materials.Num() <= 0) {
		return;
	}

	const auto materialIndex { 0 };

	// random enough without having to replicate a seed or spawn order
	const auto location = GetOwner()->GetActorLocation();
	const auto randomIndex = GetTypeHash(location) % materials.Num();
	
	const auto candidateComponents = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), tags::randomMaterial);

	for (auto candidateComponent : candidateComponents) {
		auto meshComponent = Cast<UMeshComponent>(candidateComponent);
		meshComponent->SetMaterial(materialIndex, materials[randomIndex]);
	}
}