#include "Dungeons.h"
#include "RollingDropComponent.h"

URollingDropComponent::URollingDropComponent() {}

void URollingDropComponent::BeginPlay() {
	Super::BeginPlay();

	DropChanceActor = actorquery::getFirstActor<AItemDropChanceActor>(GetWorld());
}

TArray<FNetworkedItemDropData> URollingDropComponent::GatherItemDropData(const FItemDropSource& dropSource) {
	if (DropChanceActor) {
		return DropChanceActor->RollForItemDropData(dropSource);
	}

	UE_LOG(LogTemp, Warning, TEXT("DropChanceActor is missing. Unable to roll for item drop data."));

	return {};
}
