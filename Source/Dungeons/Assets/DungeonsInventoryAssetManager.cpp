// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonsInventoryAssetManager.h"
#include "DungeonsAssetManager.h"
#include "Engine/Engine.h"


UDungeonsInventoryAssetManager::UDungeonsInventoryAssetManager()
{
	
}

UDungeonsInventoryAssetManager::~UDungeonsInventoryAssetManager()
{
	
}

void UDungeonsInventoryAssetManager::OnInventoryOpen()
{
	Super::OnInventoryOpen();
	onInventoryStateChanged.Broadcast(true);
}

void UDungeonsInventoryAssetManager::OnInventoryClosed()
{
	for (int i = 0; i < loadedObjectPaths.Num(); ++i)
	{
		AssetLoader.Unload(loadedObjectPaths[i]);
	}
	Super::OnInventoryClosed();
	onInventoryStateChanged.Broadcast(false);

	loadedObjectPaths.Empty();
	loadedObjectsNames.Empty();

}
