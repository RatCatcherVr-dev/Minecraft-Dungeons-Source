#include "Dungeons.h"
#include "EyeOfEnderDropComponent.h"
#include "DungeonsGameMode.h"
#include "game/item/ItemUtil.h"
#include "game/item/instance/EyeOfEnderInstance.h"


UEyeOfEnderDropComponent::UEyeOfEnderDropComponent()
{
}

void UEyeOfEnderDropComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthComponent* healthComponent = Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass())))
	{
		healthComponent->OnDeath.AddUObject(this, &UEyeOfEnderDropComponent::HandleDeath);
	}
}

void UEyeOfEnderDropComponent::HandleDeath()
{
	if (GetOwner()->HasAuthority())
	{
		FInventoryItemData inventoryItemData = FInventoryItemData(game::item::type::EyeOfEnder.getId());
		inventoryItemData.SubItemID = static_cast<int>(eyeOfEnderType);
		AStorableItem* eyeOfEnderItem = game::item::util::spawnStorableItem(*GetWorld(), GetOwner()->GetActorLocation(), inventoryItemData);
		eyeOfEnderItem->SetReplicates(true);
		eyeOfEnderItem->ApplyDropEffect(GetOwner());
	}
}
