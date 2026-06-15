// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "DungeonsCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsCheatManager : public UCheatManager
{
	GENERATED_BODY()

	/** Show world positions. */
	UFUNCTION(exec, BlueprintCallable, Category = "Dungeons Cheat Manager")
	virtual void ShowDebugInfo(bool toggle);

	UFUNCTION(Exec, Category = "Dev tools")
	void Suicide();

	UFUNCTION(Exec, Category = "Dev tools")
	void EndGame();
};
