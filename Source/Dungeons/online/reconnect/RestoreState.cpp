#include "Dungeons.h"
#include "RestoreState.h"
#include <AbilitySystemComponent.h>
#include <GameFramework/PlayerController.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/HealthComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "game/item/ItemTypeDefs.h"


RestoreState::RestoreState(float healthPercentage, int initialGearPower, bool hasHealthPotion, TArray<float> cooldowns)
	: HealthPercentage(healthPercentage)
	, MissionStartingGearPower(initialGearPower)
	, HasHealthPotion(hasHealthPotion)
	, Cooldowns(std::move(cooldowns)) {
}

RestoreState RestoreState::CreateFromPlayerCharacter(const APlayerCharacter* player) {
	const auto* hc = player->FindComponentByClass<UHealthComponent>();
	const auto* ec = player->GetEquipmentComponent();

	auto rs = RestoreState(hc->GetCurrentHealthPercentage(), ec->GetMissionStartingEquippedGearPower(), ec->HasHealthPotionEquipped(), ec->GetCooldowns());
	rs.StoreQuiver(ec, ESlotType::Arrow, game::item::type::Arrow.getId());
	rs.StoreQuiver(ec, ESlotType::BurningArrow, game::item::type::BurningArrow.getId());
	rs.StoreQuiver(ec, ESlotType::FireworksArrow, game::item::type::FireworksArrow.getId());
	rs.StoreQuiver(ec, ESlotType::TormentProjectile, game::item::type::TormentProjectile.getId());
	rs.StoreQuiver(ec, ESlotType::HeavyHarpoon, game::item::type::HeavyHarpoon.getId());
	rs.StoreQuiver(ec, ESlotType::VoidArrow, game::item::type::VoidArrow.getId());
	rs.StoreQuiver(ec, ESlotType::PoisonArrow, game::item::type::PoisonArrow.getId());
	return rs;
}

void RestoreState::StoreQuiver(const UEquipmentComponent* ec, ESlotType slotType, const FItemId& itemType) {
	Quivers.Emplace(Quiver(slotType, itemType, ec->GetSlot(slotType).GetCount()));
}

void RestoreState::ApplyToPlayerCharacter(APlayerCharacter* player) const {
	const auto characterAbilitySystem = player->GetAbilitySystemComponent();
	const auto* hc = player->FindComponentByClass<UHealthComponent>();

	characterAbilitySystem->SetNumericAttributeBase(UHealthAttributeSet::HealthAttribute(), hc->GetMaximumHealth() * HealthPercentage);
	auto* ec = player->FindComponentByClass<UEquipmentComponent>();
	for (const auto& q : Quivers) {
		Cast<UArrowItemSlot>(&ec->GetSlot(q.SlotType))->EquipFixedAmount(FInventoryItemData(q.ItemType, MissionStartingGearPower), q.Count);
	}

	// Set Gear power at start of mission.
	ec->SetMissionStartingEquippedGearPower(MissionStartingGearPower);

	TArray<UItemSlot*> slotsToCooldown;

	//D11.KS - Restore potion
	UItemSlot& healthPotionItemSlot = ec->GetSlot(ESlotType::HealthPotion);
	if(HasHealthPotion)
	{	
		healthPotionItemSlot.EquipItem(game::item::type::HealthPotion.getId());	
	}

	slotsToCooldown.Push(&healthPotionItemSlot);

	for(UItemSlot* activePermanentSlot : ec->GetSlotsOfType(ESlotType::ActivePermanent))
	{
		slotsToCooldown.Push(activePermanentSlot);
	}

	//D11.KS - Apply all the cooldowns we've restored.
	for(int i = 0; i < slotsToCooldown.Num() && i < Cooldowns.Num(); i++)
	{
		if(slotsToCooldown[i] && Cooldowns[i] > 0.0f)
		{
			slotsToCooldown[i]->TriggerCooldown(Cooldowns[i]);
		}
	}

}