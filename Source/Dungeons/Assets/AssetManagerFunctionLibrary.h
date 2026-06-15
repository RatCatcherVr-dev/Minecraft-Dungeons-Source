// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetManagerFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAssetManagerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Object")
	static class UDungeonsAssetManager* GetDefaultAssetManager();

	UFUNCTION(BlueprintPure, Category = "Object")
	static class UDungeonsInventoryAssetManager* GetDefaultInventoryAssetManager();

	UFUNCTION(BlueprintCallable , Category = "Object")		
	static class UStaticMeshComponent* CreateHiddenShadowMeshCopy(UStaticMeshComponent* sourceComponent);
};
