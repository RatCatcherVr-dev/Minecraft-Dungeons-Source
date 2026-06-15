#include "SatchelOfNeedInstance.h"
#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/OxygenComponent.h"
#include "util/Algo.h"

void ASatchelOfNeedInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);
	using namespace game::item::util;

	if (Role == ROLE_Authority) {
		PossibleItemsToDrop = algo::copy_if_map_tarray(GetItemRegistry().GetValues(), RETLAMBDA(it->hasTag(ItemTag::Potion)), RETLAMBDA(FSpecifiedStoreCountItemDrop(it->getId())));
		bool duplicate = (FMath::RandRange(1, 100) <= duplicateDropChance);
		int numDroppedItems = 0;

		if (CanDropWaterBreathingPotion()) {
			if (ShouldDropWaterBreathingPotion()) {
				PossibleItemsToDrop.RemoveAll([](game::item::util::FSpecifiedStoreCountItemDrop itemDrop) { return itemDrop.Id != game::item::type::WaterBreathingPotion.getId(); });
				numDroppedItems = DropItems(1, duplicate, false);
				PossibleItemsToDrop = algo::copy_if_map_tarray(GetItemRegistry().GetValues(), RETLAMBDA(it->hasTag(ItemTag::Potion)), RETLAMBDA(FSpecifiedStoreCountItemDrop(it->getId())));
				PossibleItemsToDrop.RemoveAll([](game::item::util::FSpecifiedStoreCountItemDrop itemDrop) { return itemDrop.Id == game::item::type::WaterBreathingPotion.getId(); });
			}
		}
		else {
			PossibleItemsToDrop.RemoveAll([](game::item::util::FSpecifiedStoreCountItemDrop itemDrop) { return itemDrop.Id == game::item::type::WaterBreathingPotion.getId(); });
		}

		DropItems(2 - numDroppedItems, duplicate, true);
	}
}

bool ASatchelOfNeedInstance::CanDropWaterBreathingPotion() const {
	APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
	UOxygenComponent* oxygenComponent = Cast<UOxygenComponent>(player->GetComponentByClass(UOxygenComponent::StaticClass()));

	return oxygenComponent && oxygenComponent->GetIsOxygenLow();
}

bool ASatchelOfNeedInstance::ShouldDropWaterBreathingPotion() const {
	APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
	UAbilitySystemComponent* abilityComponent = player->GetAbilitySystemComponent();

	return abilityComponent && abilityComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(("StatusEffect.Oxygen.Drowning")));
}