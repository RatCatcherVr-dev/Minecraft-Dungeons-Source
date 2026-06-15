// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../CharacterAnimInstance.h"
#include "PufferfishAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class DUNGEONS_API UPufferfishAnimInstance : public UCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(transient, BlueprintReadWrite, Category = "Dungeons|Animation")
	bool bInflate;
};
