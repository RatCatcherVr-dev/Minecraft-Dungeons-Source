// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/item/ItemSlot.h"
#include "Synergy.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API USynergy : public UEnchantment
{
	GENERATED_BODY()
public:
	void OnStart() override;
	void OnEnd() override;

protected:
	UFUNCTION()
	virtual void OnItemSuccess() {}
private:
	UFUNCTION()
	void OnItemInstanceChanged(UItemSlot* slot);

	void RefreshItems();

	TArray<TWeakObjectPtr<UItemSlot>> ItemSlots;
};


UCLASS()
class DUNGEONS_API UHealthPotionSynergy : public UEnchantment
{
	GENERATED_BODY()
public:
	void OnStart() override;
	void OnEnd() override;

protected:
	UFUNCTION()
	void OnItemSuccess(UItemSlot* slot, bool success);
	virtual void OnHealthPotionUsed() {}
};

