// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorCounterWidgetBase.h"
#include "PawnCounterWidgetBase.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UPawnCounterWidgetBase : public UActorCounterWidgetBase
{
	GENERATED_BODY()

public:	
	void OnOwnerWasChanged() override;	
};
