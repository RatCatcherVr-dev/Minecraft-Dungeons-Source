#include "Dungeons.h"
#include "RefreshSuppliesComponent.h"
#include "HealthComponent.h"
#include "../actor/character/player/PlayerCharacter.h"
#include "../item/ItemUtil.h"
#include "Components/ArrowComponent.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/item/ItemEmitterActor.h"

URefreshSuppliesComponent::URefreshSuppliesComponent() {
	bReplicates = true;
}

void URefreshSuppliesComponent::TryDropSuppliesFor(APlayerCharacter* instigator) {
	if (!GetOwner() || !GetOwner()->HasAuthority()) {
		return;
	}

	// every player can click it once?
	if (UsedBy.Contains(instigator)) {
		return;
	}
	UsedBy.Push(instigator);
	OnRep_UsedByChanged();

	FVector spawnLocation = GetOwner()->GetActorLocation();
	FVector spawnDirection;

	UActorComponent* component = GetOwner()->GetComponentByClass(UArrowComponent::StaticClass());
	if (component) {
		UArrowComponent* arrow = Cast<UArrowComponent>(component);
		spawnLocation = arrow->GetComponentLocation();
		spawnDirection = arrow->GetComponentRotation().Vector();
	}
	
	// all connected players

	//D11.SC Send off to spawner to load & spawn this
	game::item::util::FSpecifiedStoreCountItemDrop ItemDropFood(game::item::type::Food1.getId());
	game::item::util::FSpecifiedStoreCountItemDrop ItemDropArrows(game::item::type::Arrow.getId(), game::item::type::Arrow.getStoreCount() * 2);

	TArray<game::item::util::FSpecifiedStoreCountItemDrop>	GeneratedItemsData = { ItemDropFood, ItemDropArrows };

	AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), GeneratedItemsData, spawnLocation, instigator, false, true, true, FItemEmitterActorCompleteDelegate::CreateLambda([spawnDirection](TArray< AStorableItem* > & SpawnedItems) {

		for (auto item : SpawnedItems)
		{
			item->SetReplicates(true);
			item->SetActorTickEnabled(true);			
		}

		game::item::util::SpreadOutItemsInSquareWithDirection(SpawnedItems, spawnDirection, FloatRange(0.0f, 1.0f));
	}));
}

void URefreshSuppliesComponent::OnRep_UsedByChanged() {
	OnUsedByChanged.Broadcast(UsedBy);
}

void URefreshSuppliesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URefreshSuppliesComponent, UsedBy);
}
