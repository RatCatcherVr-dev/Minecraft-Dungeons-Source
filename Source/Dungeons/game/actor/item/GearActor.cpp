#include "Dungeons.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "GearActor.h"

AGearActor::AGearActor() {
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Paperdoll = CreateDefaultSubobject<UPaperdollComponent>(TEXT("Paperdoll"));

	//D11.SC Set the Paperdoll as the root component to stop any actual gear objects moaning about not having a root
	SetRootComponent(Paperdoll);

	NetUpdateFrequency = 2.0f;
	MinNetUpdateFrequency = 1.0f;
}

void AGearActor::BeginPlay() {
	Super::BeginPlay();
	if (Paperdoll) {
		Paperdoll->OnAttachedToPlayerMesh.AddUObject(this, &AGearActor::OnDetachedComponentCreated);
	}
}

void AGearActor::OnDetachedComponentCreated(USceneComponent* component) {
	if (component) {
		component->ComponentTags.Add(UEquipmentDisplayComponent::SlotTypeToComponentTag(SlotType));
		DetachedComponents.Add(component);		
	}	
}

void AGearActor::SetVisibility(const bool visible) {
	for (auto childComponent : DetachedComponents) {
		childComponent->SetVisibility(visible);
	}
}

AItemActor::AItemActor() {
	SlotType = ESlotType::ActivePermanent;
}

AItemActor* AItemActor::SpawnItemActor(UWorld* world, const TSubclassOf<AItemActor> itemClass) {
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return world->SpawnActor<AItemActor>(itemClass, params);
}
