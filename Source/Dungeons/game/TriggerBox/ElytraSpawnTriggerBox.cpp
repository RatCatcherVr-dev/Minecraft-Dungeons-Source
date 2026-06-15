#include "ElytraSpawnTriggerBox.h"
#include "UObjectBase.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/ElytraComponent.h"


AElytraSpawnTriggerBox::AElytraSpawnTriggerBox() : Super()
{
	Spawnpoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Elytra Spawn Position"));
	Spawnpoint->SetupAttachment(GetRootComponent());
	Spawnpoint->SetWorldRotation(FRotator(90, 0, 0));
	GetCollisionComponent()->SetCollisionProfileName(TEXT("PlayerTrigger"));
}

void AElytraSpawnTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!spawnedItem.IsValid())
	{
		if (auto* Character = Cast<APlayerCharacter>(OtherActor))
		{
			if (Character->IsLocallyControlled() && !Character->GetElytraComponent()->IsEquipped())
			{
				game::item::util::FSpecifiedStoreCountItemDrop Item(game::item::type::Elytra.getId());
				TArray<game::item::util::FSpecifiedStoreCountItemDrop>	GeneratedItemsData = { Item };

				AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), GeneratedItemsData, Spawnpoint->GetComponentLocation(), OtherActor, false, true, false, FItemEmitterActorCompleteDelegate::CreateLambda([this](TArray< AStorableItem* > & SpawnedItems) {
					if (SpawnedItems.Num()) spawnedItem = SpawnedItems[0];
				}));
			}
		}
	}
}
