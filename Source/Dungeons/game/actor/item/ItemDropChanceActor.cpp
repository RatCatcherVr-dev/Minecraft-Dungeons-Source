#include "Dungeons.h"
#include "ItemDropChanceActor.h"

#include "DungeonsGameMode.h"
#include "game/component/EquipmentComponent.h"
#include "util/EnumUtil.h"
#include "game/component/EnchantmentComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/Game.h"
#include "game/util/DungeonsGearUtilLibrary.h"

AItemDropChanceActor::AItemDropChanceActor(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
	  , mDisabled(false) {
	
	PrimaryActorTick.bCanEverTick = false;
}

void AItemDropChanceActor::BeginPlay() {
	Super::BeginPlay();

	if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
		gameMode->ActorUsedItem.AddUObject(this, &AItemDropChanceActor::OnActorUsedItem);
	}
}

TArray<FNetworkedItemDropData> AItemDropChanceActor::RollForItemDropData(const FItemDropSource& dropSource) {
	TArray<FNetworkedItemDropData> dropData;

	bool canDropWaterBreathingPotion = true;
	if (ABaseCharacter* triggeringCharacter = Cast<ABaseCharacter>(dropSource.TriggeringPlayer)) {
		if (!triggeringCharacter->IsUnderwater()) {
			canDropWaterBreathingPotion = false;
		}
	}

	for (FDropCategoryDescription& description : RollForDropCategories(dropSource)) {
		if (description.Category == EDropCategory::Consumable && !canDropWaterBreathingPotion) {
			description.Category = EDropCategory::ConsumableNoWaterBreathing;
		}
		if (const auto drop = description.GetDropData()) {
			dropData.Add(drop.GetValue());
		}
	}

	return dropData;
}

TArray<FDropCategoryDescription> AItemDropChanceActor::RollForDropCategories(const FItemDropSource& dropSource) {
	TArray<FDropCategoryDescription> rolledForItemCategories;

	if (IsAllowedToRoll()) {		
		auto anyRollWasSuccessful = false;
		for (auto& drop : Drops) {
			const auto shouldRoll = !anyRollWasSuccessful || !drop.Exclusive;
			if (shouldRoll) {
				const auto probability = CalculateDropProbability(drop, dropSource.TriggeringPlayer);

				auto rollWasSuccessful = IsRollSuccessful(drop, probability);

				const auto shouldReroll = !rollWasSuccessful && !anyRollWasSuccessful;
				if (shouldReroll) {
					rollWasSuccessful = IsRerollSuccessful(drop, dropSource, probability);
				}

				if (rollWasSuccessful) {
					rolledForItemCategories.Add(drop);
					ResetDroprate(drop, dropSource.TriggeringPlayer);
					anyRollWasSuccessful = true;	
				}
			}
		}
	}

	return rolledForItemCategories;
}

float AItemDropChanceActor::CalculateDropProbability(const FDropCategoryDescription& drop, APlayerCharacter* playerCharacter) {
	float alpha = 0;
	const auto now = GetWorld()->GetRealTimeSeconds();

	if (playerCharacter && DroprateData.Contains(playerCharacter)) {
		const auto& dropRateModifierForCategory = DroprateData[playerCharacter].Get(drop.Category);
		alpha = FMath::Clamp((drop.Category == EDropCategory::Food) ? dropRateModifierForCategory.Get(0.0f) : ((now - dropRateModifierForCategory.Get(now)) / drop.TimeToMaxSeconds), 0.f, 1.f);

		const auto timestamp = DroprateData[playerCharacter].Get(drop.Category).Get(now);
		UE_LOG(LogDungeons, Verbose, TEXT("Interpolationg for %s at time %f with timestamp %f and alpha result of %f."), 
			*GetEnumValueToString(drop.Category), now, timestamp, alpha);
	}

	return FMath::Lerp(drop.MinProbability, drop.MaxProbability, alpha);
}

bool AItemDropChanceActor::IsRollSuccessful(const FDropCategoryDescription& drop, const float probability) {
	const auto sample = FMath::FRand();
	UE_LOG(LogDungeons, Verbose, TEXT("Rolling for %s with requested probability %f and sample %f."), 
		*GetEnumValueToString(drop.Category), probability, sample);
	return sample < probability;
}

bool AItemDropChanceActor::IsRerollSuccessful(const FDropCategoryDescription& drop, const FItemDropSource& dropSource, const float probability) {
	if (dropSource.TriggeringPlayer) {
		return DungeonsGearUtilLibrary::OnAfterDropRolled(dropSource.TriggeringPlayer, drop, dropSource, probability);
	}
	return false;
}

void AItemDropChanceActor::ResetDroprate(const FDropCategoryDescription& drop, APlayerCharacter* playerCharacter) {
	if (playerCharacter && DroprateData.Contains(playerCharacter)) {
		DroprateData[playerCharacter].Reset(drop.Category, GetWorld()->GetRealTimeSeconds());
	}
}

bool AItemDropChanceActor::IsAllowedToRoll() const {
	return !mDisabled && HasAuthority();
}

void AItemDropChanceActor::RegisterPlayer(APlayerCharacter* playerCharacter) {
	const auto now = GetWorld()->GetTimeSeconds();

	DroprateData.Add(playerCharacter, {now});

	if (const auto* equipmentComponent = playerCharacter->FindComponentByClass<UEquipmentComponent>()) {
		auto& arrowSlot = equipmentComponent->GetSlot(ESlotType::Arrow);

		arrowSlot.OnCountReplicated.AddDynamic(this, &AItemDropChanceActor::OnSlotCountChanged);

		OnSlotCountChanged(&arrowSlot);
	}

	if (auto* healthComponent = playerCharacter->GetHealthComponent()) {
		healthComponent->OnDamageReceived.AddUObject(this, &AItemDropChanceActor::OnPlayerHealthChanged, playerCharacter);
		healthComponent->OnHeal.AddUObject(this, &AItemDropChanceActor::OnPlayerHealthChanged, playerCharacter);
	}
}

void AItemDropChanceActor::UnRegisterPlayer(APlayerCharacter* playerCharacter) {
	DroprateData.Remove(playerCharacter);

	const auto* equipmentComponent = playerCharacter->FindComponentByClass<UEquipmentComponent>();
	if (equipmentComponent) {
		auto arrowSlots(equipmentComponent->GetSlotsOfType(ESlotType::Arrow));
		auto healthPotionSlots(equipmentComponent->GetSlotsOfType(ESlotType::HealthPotion));

		arrowSlots.Last()->OnCountReplicated.RemoveDynamic(this, &AItemDropChanceActor::OnSlotCountChanged);
		healthPotionSlots.Last()->OnCountReplicated.RemoveDynamic(this, &AItemDropChanceActor::OnSlotCountChanged);
	}
}

void AItemDropChanceActor::OnActorUsedItem(const AActor* actor, const AItemInstance* itemInstance) {
	if (const auto player = Cast<const APlayerCharacter>(actor)) {
		const auto& itemType = itemInstance->GetItemType();
		if (itemType.isConsumable() && itemType.isInstant()) {
			check(DroprateData.Contains(player) && "Player should be registerd with item drop actor by now...");
			DroprateData[player].ConsumableUseTimestamp = GetWorld()->GetTimeSeconds();
			UE_LOG(LogDungeons, Verbose, TEXT("Setting consumable timestamp for player %i."), player->GetDungeonsBasePlayerState()->GetPlayerNumber());
		}
	}
 }

void AItemDropChanceActor::OnSlotCountChanged(UItemSlot* slot) {
	if (const auto player = Cast<APlayerCharacter>(slot->GetOwner())) {
		if (DroprateData.Contains(player)) {
			if (slot->SlotTypeId == ESlotType::Arrow) {
				OnArrowSlotChanged(slot, player);
			} 
		}
	}
}

void AItemDropChanceActor::OnPlayerHealthChanged(float amount, APlayerCharacter* playerCharacter) {
	auto& healthFraction = DroprateData[playerCharacter].HealthFraction;
	if (healthFraction.IsSet()) {
		if (auto* healthComponent = playerCharacter->GetHealthComponent()) {
			auto healthPercentage = healthComponent->GetCurrentHealthPercentage();
			healthFraction = 1.0f - healthPercentage;
		}
	}
}

void AItemDropChanceActor::OnArrowSlotChanged(UItemSlot* slot, APlayerCharacter* playerCharacter) {
	const auto arrowSlot = Cast<UArrowItemSlot>(slot);
	const auto dropLimit = arrowSlot ? FMath::RoundToInt(arrowSlot->GetStackConversionFactor() * ArrowCountDropLimit) : ArrowCountDropLimit;
	if (DroprateData[playerCharacter].ArrowLimitTimestamp.IsSet() && slot->GetCount() > dropLimit) {
		DroprateData[playerCharacter].ArrowLimitTimestamp.Reset();
	}
	else if (!DroprateData[playerCharacter].ArrowLimitTimestamp.IsSet() && slot->GetCount() <= ArrowCountDropLimit) {
		DroprateData[playerCharacter].ArrowLimitTimestamp = GetWorld()->GetTimeSeconds();
	}
}
