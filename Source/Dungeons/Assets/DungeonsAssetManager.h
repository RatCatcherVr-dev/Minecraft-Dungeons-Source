// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "DungeonsAssetManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectsLoaded);

/**
 *
 */
UCLASS()
class DUNGEONS_API UDungeonsAssetManager : public UObject
{
	GENERATED_BODY()

public:
	UDungeonsAssetManager();
	~UDungeonsAssetManager();

public:
	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	void LoadSyncAssetFromPath(FSoftObjectPath assetReference);


	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	void UnloadAllObjects(bool bCheckLoading = false);

	void OnObjectLoaded();

	void AddLoadingObjectPath(FSoftObjectPath path);

	void OnLoadingStarted();
	void OnLoadingComplete();
	bool IsInLoading();

public:

	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	class USoundBase* GetSoundFromString(FString assetPathString);

	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	class UTexture* GetTextureFromString(FString assetPathString);

public:
	UFUNCTION(BlueprintPure, Category = "DungeonsAssetManager")
	bool IsEditor();

	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	virtual void OnInventoryOpen();

	UFUNCTION(BlueprintCallable, Category = "DungeonsAssetManager")
	virtual void OnInventoryClosed();

	UPROPERTY(BlueprintAssignable, Category = "DungeonsAssetManager")
	FOnObjectsLoaded OnObjectsLoaded;

protected:
	FStreamableManager AssetLoader;
	TArray<FSoftObjectPath> loadedObjectPaths;
	TArray<FString> loadedObjectsNames;
private:

	TArray<FSoftObjectPath> LoadingLoadedObjectPaths;
	
	bool bIsLoading;
};
