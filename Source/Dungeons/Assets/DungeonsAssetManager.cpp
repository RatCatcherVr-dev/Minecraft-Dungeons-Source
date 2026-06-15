// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonsAssetManager.h"
#include "Engine/StreamableManager.h"
#include "NoExportTypes.h"
#include "SoftObjectPtr.h"
#include "Sound/SoundCue.h"
#include "UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/Texture.h"


UDungeonsAssetManager::UDungeonsAssetManager()
{
	
}

UDungeonsAssetManager::~UDungeonsAssetManager()
{
	
}

void UDungeonsAssetManager::LoadSyncAssetFromPath(FSoftObjectPath assetReference)
{
	if(assetReference.IsValid() && !loadedObjectsNames.Contains(assetReference.GetAssetName()))
	{
		if (!IsEditor())
		{
			loadedObjectPaths.Add(assetReference);
			loadedObjectsNames.Add(assetReference.GetAssetName());
			UE_LOG(LogTemp, Log, TEXT("%s loaded. We now have %d objects loaded"), *FString(assetReference.GetAssetName()), loadedObjectsNames.Num());
		}
		AssetLoader.RequestSyncLoad(assetReference, true);

	}
}

void UDungeonsAssetManager::OnObjectLoaded()
{
	OnObjectsLoaded.Broadcast();
}

void UDungeonsAssetManager::UnloadAllObjects(bool bCheckLoading)
{
	for (int i = 0; i < loadedObjectPaths.Num(); ++i)
	{
		if (!bCheckLoading || !LoadingLoadedObjectPaths.Contains(loadedObjectPaths[i]))
		{
			AssetLoader.Unload(loadedObjectPaths[i]);
		}
	}
	loadedObjectPaths.Empty();
	loadedObjectsNames.Empty();

	if (bCheckLoading)
	{
		loadedObjectPaths.Append(LoadingLoadedObjectPaths);
	}
	GEngine->PerformGarbageCollectionAndCleanupActors();
}

void UDungeonsAssetManager::AddLoadingObjectPath(FSoftObjectPath path)
{
	LoadingLoadedObjectPaths.Add(path);
}

void UDungeonsAssetManager::OnLoadingStarted()
{
	bIsLoading = true;
}

void UDungeonsAssetManager::OnLoadingComplete()
{
	bIsLoading = false;
	for (int i = 0; i < LoadingLoadedObjectPaths.Num(); ++i)
	{
		AssetLoader.Unload(LoadingLoadedObjectPaths[i]);
	}
	LoadingLoadedObjectPaths.Empty();
	GEngine->ForceGarbageCollection(true);
}

bool UDungeonsAssetManager::IsInLoading()
{
	return bIsLoading;
}

USoundBase* UDungeonsAssetManager::GetSoundFromString(FString assetPathString)
{
	FSoftObjectPath objectPath = FSoftObjectPath(assetPathString);
	if (!loadedObjectsNames.Contains(assetPathString))
	{
		if (!IsEditor())
		{
			loadedObjectPaths.Add(objectPath);
			loadedObjectsNames.Add(assetPathString);
			UE_LOG(LogTemp, Log, TEXT("%s loaded. We now have %d objects loaded"), *assetPathString, loadedObjectsNames.Num());
		}


		AssetLoader.RequestSyncLoad(objectPath, true);
	}

	USoundBase* sound = Cast<USoundBase>(objectPath.ResolveObject());

	return sound;
}

UTexture* UDungeonsAssetManager::GetTextureFromString(FString assetPathString)
{
	FSoftObjectPath objectPath = FSoftObjectPath(assetPathString);
	if (!loadedObjectsNames.Contains(assetPathString))
	{
		if (!IsEditor())
		{
			loadedObjectPaths.Add(objectPath);
			loadedObjectsNames.Add(assetPathString);
			UE_LOG(LogTemp, Log, TEXT("%s loaded. We now have %d objects loaded"), *assetPathString, loadedObjectsNames.Num());
		}


		AssetLoader.RequestSyncLoad(objectPath, true);
	}

	UTexture* texture = Cast<UTexture>(objectPath.ResolveObject());

	return texture;
}

bool UDungeonsAssetManager::IsEditor()
{
	return GIsEditor;
}

void UDungeonsAssetManager::OnInventoryOpen()
{
	for (int i = 0; i < loadedObjectPaths.Num(); ++i)
	{
		AssetLoader.Unload(loadedObjectPaths[i]);
	}

	loadedObjectPaths.Empty();
	loadedObjectsNames.Empty();

	//GEngine->PerformGarbageCollectionAndCleanupActors();

}

void UDungeonsAssetManager::OnInventoryClosed()
{
	loadedObjectPaths.Empty();
	loadedObjectsNames.Empty();

	//GEngine->PerformGarbageCollectionAndCleanupActors();
}
