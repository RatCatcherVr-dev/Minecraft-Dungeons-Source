// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "util/FloatRange.h"
#include "game/item/SerializableItemId.h"
#include "game/item/ItemUtil.h"
#include "StorableDropBaseEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UStorableDropBaseEnchantment : public UEnchantment
{
	GENERATED_BODY()
protected:
	void OnStart() override;

	TArray<game::item::util::FSpecifiedStoreCountItemDrop> PossibleItemsToDrop;

	virtual int GetDropAmount() const;
private:
	UFUNCTION()
	void OnHealthPotionActivated(UItemSlot* slot, bool success);
	bool DropItems();
};
