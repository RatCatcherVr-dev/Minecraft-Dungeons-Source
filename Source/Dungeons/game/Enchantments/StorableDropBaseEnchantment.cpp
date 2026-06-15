// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "StorableDropBaseEnchantment.h"
#include "game/item/ItemUtil.h"
#include <AbilitySystemComponent.h>
#include "game/component/EquipmentComponent.h"
#include "util/FloatRange.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/Game.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/actor/item/StorableItem.h"

void UStorableDropBaseEnchantment::OnStart() {
	Super::OnStart();
	if (const auto ownerEquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		auto slots = ownerEquipmentComponent->GetSlotsOfType(ESlotType::HealthPotion);
		if (slots.Num()) {
			auto slot = slots[0];
			if (!slot->OnItemSlotActivationCompleted.Contains(this, "OnHealthPotionActivated")) {
				slot->OnItemSlotActivationCompleted.AddDynamic(this, &UStorableDropBaseEnchantment::OnHealthPotionActivated);
			}
		}
	}
}

int UStorableDropBaseEnchantment::GetDropAmount() const {
	return LevelMultiplier(Level);
}

void UStorableDropBaseEnchantment::OnHealthPotionActivated(UItemSlot* slot, bool success) {
	if (GetOwner()->HasAuthority() && success) {
		if (DropItems()) {
			BroadcastEnchantmentTriggeredEvent();

			auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
			FGameplayCueParameters params;
			params.Location = GetOwner()->GetActorLocation();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.FoodReserves"), params);
		}
	}
}

bool UStorableDropBaseEnchantment::DropItems() {
	auto dropAmount = GetDropAmount();

	if (dropAmount > 0) {
		using namespace game::item::util;
		// D11.DB - Ensure mission restrictions apply (some drops are restricted by DLC levels).
		TArray<FSpecifiedStoreCountItemDrop> possibleItems;
		if (const auto game = actorquery::getGame(GetWorld())) {
			for (auto& item : PossibleItemsToDrop) {
				const auto& type = GetItemRegistry().Get(item.Id);
				if (type.isAllowedOnMissionDifficulty(game->settings().levelName, game->settings().difficulty)) {
					possibleItems.Add(item);
				}
			}
		}
				
		//D11.SC pregenerate a drop list and pass it off to the emitter to load and process
		if (possibleItems.Num() > 0)
		{
			TArray<FSpecifiedStoreCountItemDrop>	GeneratedItemsData;

			for (int i = 0; i < dropAmount; ++i) {
				int index = FMath::RandRange(0, possibleItems.Num() - 1);
				GeneratedItemsData.Push(possibleItems[index]);
			}
			
			AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), GeneratedItemsData, GetOwner()->GetActorLocation(), GetOwner(), false, false, true, FItemEmitterActorCompleteDelegate::CreateLambda([](TArray< AStorableItem* > & SpawnedItems) {

				for (auto item : SpawnedItems)
				{
					item->SetReplicates(true);
					item->SetActorTickEnabled(true);
				}

				SpreadOutItemsInSquare(SpawnedItems);

			}));

			return true;
		}

		
	}

	return false;
}
