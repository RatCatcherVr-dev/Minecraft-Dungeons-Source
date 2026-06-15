// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Assets/DungeonsAssetManager.h"
#include "DungeonsInventoryAssetManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryStateChanged, bool, bIsInInventory);

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsInventoryAssetManager : public UDungeonsAssetManager
{
	GENERATED_BODY()
	
public:
	UDungeonsInventoryAssetManager();
	~UDungeonsInventoryAssetManager();

	UPROPERTY(BlueprintAssignable, Category = "DungeonsInventoryAssetManager")
	FOnInventoryStateChanged onInventoryStateChanged;


	virtual void OnInventoryOpen() override;


	virtual void OnInventoryClosed() override;
};
